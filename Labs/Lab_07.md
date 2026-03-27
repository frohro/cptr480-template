# CPTR 480 — Lab 7 ★
## UAC2 + CDC Composite Device — Part 2: Live Audio + Quisk SDR Milestone
**Date:** Tuesday, May 12, 2026 | 2:00–5:00 p.m.
**Due:** All deliverables committed to your GitHub repo by **2 p.m., Tuesday May 19**

---

### Learning Objectives
By the end of this lab you will be able to:
- Implement a TinyUSB UAC2 audio TX callback that feeds PCM1808 DMA data to the USB isochronous pipe
- Design a thread-safe inter-core handoff from the DMA ISR (Core 1) to the USB task (Core 0)
- Compute and send the UAC2 feedback value for Full Speed asynchronous isochronous
- Observe your SDR's waterfall in Quisk using audio streamed entirely from your own firmware

> **★ SDR Milestone.** Completing this lab means your Phase 1 project is done: Si5351a LO control + UAC2 stereo I/Q audio + CDC tune commands, all from the YD-RP2040, received and displayed by Quisk. Everything after this is extension work.

---

### What You Need
- 2026 board with YD-RP2040
- Pico Debug Stack
- Lab 4 PIO I2S + DMA code
- Lab 6 UAC2 + CDC composite firmware as the starting point
- Lab 5 Quisk hardware driver (`quisk_hardware_wwusdr.py`)
- Host PC with Quisk source (`../../quisk/quisk.py`)
- Logic analyzer (verify DMA timing if audio sounds garbled)

---

### Overview

In Lab 6 the UAC2 audio interface enumerated but no samples moved. Today you wire the PCM1808 DMA pipeline from Lab 4 into the TinyUSB audio TX path:

```
PCM1808 → PIO I2S → DMA ping-pong (Core 1 ISR)
                           │  inter-core FIFO (pointer)
                           ↓
                    TinyUSB audio TX callback (Core 0)
                           │  tud_audio_write()
                           ↓
                    USB isochronous IN endpoint
                           │
                           ↓
                    Quisk snd-usb-audio capture
                           │
                           ↓
                    Quisk waterfall ★
```

---

## Part 1 — Merge DMA Code into the Lab 6 Project (~25 min)

1. Copy `i2s_rx.pio`, your DMA init code, and `si5351.h/.c` from Labs 3 and 4 into `lab7/`.
2. Update `CMakeLists.txt`:
   ```cmake
   target_link_libraries(lab7
       pico_stdlib tinyusb_device tinyusb_board
       hardware_i2c hardware_pio hardware_dma
       pico_multicore)
   pico_generate_pio_header(lab7 ${CMAKE_CURRENT_LIST_DIR}/i2s_rx.pio)
   ```
3. Confirm the project builds before making any logic changes.

---

## Part 2 — Inter-Core Audio Handoff Design (~20 min)

The DMA completion ISR runs on Core 1. TinyUSB's audio callback runs on Core 0. You need a safe hand-off.

**Recommended design — pointer FIFO:**

```
Core 1 DMA ISR:
  1. Ping buffer is full → chain starts filling Pong.
  2. Push pointer to Ping buffer into multicore FIFO:  multicore_fifo_push_timeout_us(ptr, 0)
     (non-blocking: if FIFO is full, drop the buffer — see below)

Core 0 USB audio callback  tud_audio_tx_done_pre_load_cb():
  1. Check if FIFO has a pointer:  multicore_fifo_pop_timeout_us(&ptr, 0)
  2. If yes: copy samples from ptr into TinyUSB via tud_audio_write()
  3. If no: send silence (zeros) — USB must always fill the isochronous slot
```

**Why non-blocking FIFO push?** If Core 0 is slow (USB interrupt storm, debug output), Core 1 must not stall the DMA ISR. Dropped buffers produce a brief click; a stalled ISR produces a DMA overrun and corrupts all subsequent audio.

Draw this data flow in your `lab7/lab7_report.md` before implementing it.

> **Buffer sizing check:** At 48 kHz stereo 32-bit, each 1 ms USB frame needs 48 × 2 × 4 = 384 bytes = `WORDS_PER_BUF × sizeof(uint32_t)`. Your DMA ping-pong buffers (`STEREO_FRAMES=48`) hold exactly 1 USB frame (384 bytes). The DMA ISR fires every 1 ms; the Core 0 audio callback also fires every 1 ms. This is a clean **1:1 relationship** — each DMA completion produces exactly one USB frame's worth of audio, with no partial-frame arithmetic.

---

## Part 3 — Implement the TinyUSB Audio TX Callback (~50 min)

TinyUSB calls this function before loading the next isochronous IN transfer:

```c
bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting) {
    (void)rhport; (void)itf; (void)ep_in;

    if (cur_alt_setting == 0) return true;   // host not streaming yet

    uint32_t ptr_val = 0;
    bool got = multicore_fifo_pop_timeout_us(0, &ptr_val);

    if (got && ptr_val != 0) {
        // ptr points to buf_a or buf_b: WORDS_PER_BUF uint32_t words = 384 bytes.
        // Format: L0, R0, L1, R1, ... — left-justified 24-bit in 32-bit container.
        // Linux snd-usb-audio with bSubslotSize=4/bBitResolution=24 expects this.
        tud_audio_write((const void *)(uintptr_t)ptr_val,
                        WORDS_PER_BUF * sizeof(uint32_t));  // 384 bytes
    } else {
        // Silence — must still fill the slot or the host sees a short packet.
        static uint32_t silence[WORDS_PER_BUF];  // zero-init'd by C
        tud_audio_write(silence, sizeof(silence));
    }

    return true;
}
```

> **Sample format:** PCM1808 delivers 24-bit samples left-justified in a 32-bit frame from the PIO program. TinyUSB sends raw bytes; the host (Linux `snd-usb-audio`) interprets based on `bSubslotSize=4` / `bBitResolution=24`. Linux expects the sample in the **most significant 24 bits** of the 32-bit word (left-justified), which is exactly what the PCM1808 PIO program delivers. No shifting needed.

---

## Part 4 — Async vs Adaptive Isochronous and the Feedback Endpoint (~25 min)

This part explains how the `AUDIO_USE_ASYNC_FEEDBACK` compile-time switch in `tusb_config.h` controls the USB isochronous transfer mode and whether a feedback endpoint is included in the descriptor.

### USB Isochronous Synchronization Types

USB audio uses three isochronous synchronization types, encoded in `bmAttributes[3:2]` of the data endpoint:

| Type | bmAttributes | Feedback EP | Meaning |
|---|---|---|---|
| Synchronous | 0x0D | No | Device clocks from USB SOF |
| Adaptive | 0x09 | No | Device *adapts* its clock to match host |
| Asynchronous | 0x05 | Yes (required) | Host *adapts* read rate via feedback |

With **Async** (`AUDIO_USE_ASYNC_FEEDBACK=1`):
- The data EP is declared `bmAttributes=0x05`.
- A 3-byte Q10.14 feedback endpoint (EP1 OUT) is included in the descriptor.
- The host reads the feedback value to know how many samples/frame to request.
- Q10.14 nominal value for 48 kHz: `48 × 2^14 = 786432 = 0x0C0000 → {0x00, 0x00, 0x0C}` LE.
- **Current limitation:** TinyUSB 0.18 (Pico SDK 2.1.1) cannot service the feedback EP (`audiod_function_t` lacks `ep_fb`). The descriptor declares the EP, but no feedback packets are sent. Linux `snd-usb-audio` tolerates this for capture-only devices.

With **Adaptive** (`AUDIO_USE_ASYNC_FEEDBACK=0`):
- The data EP is declared `bmAttributes=0x09`.
- No feedback EP is included (descriptor shrinks from 125 → 118 bytes).
- The spec says the device should adapt its clock to the host SOF, but we don't truly do this — the Si5351a runs at a fixed rate.
- All three OSes (Linux, Windows, macOS) broadly accept Adaptive mode for capture-only.<br>This is the **cross-platform recommended setting** for student boards.

### The `AUDIO_FEEDBACK_APPROACH` Selector

In addition to `AUDIO_USE_ASYNC_FEEDBACK`, `tusb_config.h` defines `AUDIO_FEEDBACK_APPROACH` which selects the firmware implementation:

```c
#define AUDIO_FEEDBACK_APPROACH  3   // active with this SDK
```

| Value | Mechanism | SDK required |
|---|---|---|
| 1 | `tud_audio_feedback_params_cb()` | TinyUSB ≥ 0.20 / SDK ≥ 2.2.1 |
| 2 | `tud_audio_fb_set()` | Intermediate TinyUSB |
| 3 | No firmware feedback (descriptor-only) | TinyUSB 0.18 / SDK 2.1.1 |

`main.c` wraps all three implementations in `#if AUDIO_FEEDBACK_APPROACH == N` guards so students can see the full picture. Only Approach 3 compiles today.

### Changing the Mode

To switch from the default ASYNC to ADAPTIVE, edit **one line** in `lab7/tusb_config.h`:
```c
#define AUDIO_USE_ASYNC_FEEDBACK  0   // was 1
```
`CFG_TUD_AUDIO_FUNC_1_DESC_LEN` will automatically become 118, and the data EP will advertise Adaptive. No other files need to change.

> **Report exercise:** Even though we cannot send feedback packets in this SDK, calculate the Q10.14 nominal value for your sample rate and verify it matches `0x0C0000`. Show your work. What would change if the sample rate were 96 kHz?

---

## Part 5 — First Audio Test (~20 min)

Before connecting Quisk, verify audio data is flowing:

1. Flash the firmware.
2. On the host:
   ```bash
   arecord -D hw:<card>,0 -f S32_LE -r 48000 -c 2 -d 3 test.wav
   ```
   Replace `<card>` with the card number shown by `arecord -l`.
3. Play it back:
   ```bash
   aplay test.wav
   ```
   You should hear noise (the PCM1808 with its inputs floating picks up board noise — that is correct). If you have a signal source, connect it to the board's RF input and you should hear a tone shift as you tune over it.
4. Check for clicks (dropped buffers) or silence (FIFO not working). If clicks: your DMA ISR is not pushing to the FIFO. If silence: the TinyUSB callback is taking the `else` branch every time — add a `printf` to debug.

> **Deliverable:** `lab7/test_audio.png` — screenshot of the waveform in Audacity (or `arecord -l` output + note of what you heard).

---

## Part 6 — Quisk Waterfall ★ (~30 min)

Update `quisk_conf_wwusdr.py` (in `/quisk/`) to use your UAC2 device instead of the borrowed sound card:

```python
# Replace the borrowed sound card name with your UAC2 device.
# Find the name with: arecord -l | grep <your product string>
# Example: "alsa:WWU SDR"  or  "hw:2,0"
name_of_sound_capt = "alsa:WWU SDR"   # <-- use your product string fragment

sample_rate = 48000
channel_i   = 0
channel_q   = 1
```

Return the borrowed sound card to the instructor.

Launch Quisk:
```bash
cd ../../quisk
python3 quisk.py --config quisk_conf_wwusdr.py
```

Tune around the AM broadcast band (500–1700 kHz) or the 40m amateur band (7.0–7.3 MHz). You should see stations in the waterfall.

> **Milestone deliverable:** `lab7/quisk_milestone.png` — screenshot of the Quisk waterfall with your UAC2 device selected as the sound source and an RF signal visible. This screenshot goes in your final project presentation.

---

## Part 7 — Commit and Phase 1 Wrap-Up (~15 min)

With Lab 7 done, your Phase 1 project is complete. Before moving to extension work:

1. Ensure every Lab 1–7 deliverable is committed and pushed.
2. Tag the commit: `git tag phase1-complete && git push origin phase1-complete`
3. Fill in `lab7/lab7_report.md`:
   - Data flow diagram (Core 1 DMA ISR → FIFO → Core 0 USB callback)
   - Q10.14 feedback value calculation: show that 48 × 2^14 = 786432 = 0x0C0000 (even though we don't send it in this SDK, understanding it will matter when SDK 2.2.1 arrives)
   - Any audio glitches observed and their cause
   - Reflection: what would break first if you increased the sample rate to 96 kHz?

---

### Troubleshooting Guide

| Symptom | Likely cause |
|---|---|
| `arecord` gives "no such device" | UAC2 enumeration failed — recheck Lab 6 descriptors |
| Audio plays but is silent | TinyUSB callback always takes the silence branch; add debug print |
| Loud clicks every ~1 second | FIFO overflow: Core 0 is not consuming fast enough; check `tud_task()` poll rate |
| `dmesg` shows `cannot submit urb` | Feedback EP in descriptor but `CFG_TUD_AUDIO_ENABLE_FEEDBACK_EP=0`; try `AUDIO_USE_ASYNC_FEEDBACK=0` (Adaptive mode) |
| Quisk shows flat noise spectrum, no signals | I/Q channels swapped (`channel_i`/`channel_q`) or PCM1808 MODE pin incorrect (stereo vs mono) |
| Quisk waterfall has image rejection problems | I and Q amplitudes unequal or not 90° apart — hardware calibration issue, note it in your report |

---

### Commit Checklist

By 2:00 p.m., Tuesday May 19:

- [ ] `lab7/CMakeLists.txt`
- [ ] `lab7/tusb_config.h`
- [ ] `lab7/usb_descriptors.c`
- [ ] `lab7/i2s_rx.pio`
- [ ] `lab7/main.c` — full audio pipeline: DMA ISR → FIFO → TinyUSB audio TX callback + CDC
- [ ] `lab7/si5351.h` and `lab7/si5351.c`
- [ ] `lab7/test_audio.png` — arecord capture screenshot or waveform
- [ ] `lab7/quisk_milestone.png` — **Quisk waterfall with UAC2 audio from your firmware ★**
- [ ] `lab7/lab7_report.md` — data flow diagram, feedback calculation, glitch analysis, 96 kHz reflection
- [ ] Git tag `phase1-complete` pushed to GitHub
