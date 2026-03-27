# CPTR 480 — Lab 8
## Extension Project Launch
**Date:** Tuesday, May 19, 2026 | 2:00–5:00 p.m.
**Due:** Extension work is ongoing — see checklist below for today's required commits

---

### Learning Objectives
By the end of this lab you will be able to:
- Translate an extension project proposal into a concrete Git branch with an actionable plan
- Identify the riskiest technical unknown in your project and design a spike to de-risk it
- Check out and configure alternative hardware (Pico W, Pico 2, Pico 2W) if your project requires it

---

### What You Need
- Your completed Lab 7 Phase 1 code (milestone tag `phase1-complete` pushed)
- Your Assignment 7 extension project proposal
- Pico W / Pico 2 / Pico 2W hardware if your project needs it (check out from instructor today)
- Any additional components your project requires (discuss with instructor)

---

### Overview

This is the kickoff lab for Phase 2. The three-hour session has two very different uses depending on where you are:

**If Lab 7 is fully working:** You should spend the bulk of today getting your extension branch started and your first spike running.

**If Lab 7 is not yet working:** Use today to finish it. A working Phase 1 SDR is the foundation for every extension option and for a strong final demo. Instructor will help you unblock. The extension project for your team may be: _finishing the base SDR to a high standard_ — that is a legitimate Phase 2 project.

---

## Part 1 — Phase 1 Status Review (~15 min)

Answer these questions in `lab8/lab8_report.md` before writing any new code:

1. Is `phase1-complete` tagged and pushed? (yes / no — if no, do it now)
2. Does Quisk show a live waterfall using your UAC2 firmware? (yes / no)
3. Does CDC tune control work? (yes / no)
4. What is the observed audio sample rate accuracy — does Quisk report underruns or overruns?
5. If something is broken, what is the specific failure and your plan to fix it this lab?

---

## Part 2 — Extension Branch Setup (~15 min)

From your main/master branch (Phase 1 complete):

```bash
git checkout -b extension/<short-name>
# Examples:
#   extension/wifi-udp-stream
#   extension/96khz-audio
#   extension/music-player
#   extension/standalone-receiver
```

Push the branch:
```bash
git push -u origin extension/<short-name>
```

Create `lab8/EXTENSION_PLAN.md` with the following sections (expand from your Assignment 7 proposal):

```markdown
# Extension Project Plan

## Project title

## Team members

## Chosen direction
(One paragraph — what does the finished extension do that Phase 1 doesn't?)

## Hardware
- Primary: YD-RP2040 on 2026 board
- Additional: (e.g., Pico W checked out from instructor, external DAC module, etc.)

## Milestones
| Week | Goal | Success criterion |
|------|------|-------------------|
| 8 (today) | Spike: [most risky thing] | [observable outcome] |
| 9 | [midpoint goal] | [observable outcome] |
| 10 | [final demo state] | [observable outcome] |

## Risks and mitigations
(At least 2 specific risks from your Assignment 7 proposal, with your current mitigation plan)

## What "done" looks like at the final demo
(One concrete paragraph — what will the audience see/hear?)
```

---

## Part 3 — Hardware Checkout and Configuration (~20 min)

If your extension requires a Pico W, Pico 2, or Pico 2W:

1. Check one out from the instructor (sign the checkout sheet).
2. Wire it to power and run the blink example to confirm it works.
3. Note in your plan which pins you will use and how it connects to the 2026 board.

**Pico W specifics:**
- CYW43 WiFi chip shares SPI with GPIO 23/24/25 — these are NOT available as general GPIO.
- `pico_cyw43_arch` must be initialized before any WiFi use; this takes ~200 ms.
- The onboard LED is on the CYW43, accessed via `cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1)` — not `gpio_put(25, 1)`.

**Pico 2 / Pico 2W specifics:**
- RP2350 has 8 PIO state machines vs RP2040's 8 (same count, but two PIO blocks with 4 each).
- Floating-point is hardware-accelerated on RP2350.
- The SDK target changes: `set(PICO_BOARD pico2)` in CMakeLists.txt.

---

## Part 4 — Risk Spike (~90 min)

The most valuable thing you can do today is attack the riskiest unknown in your project.

**Examples by extension direction:**

| Extension | Likely riskiest unknown | Suggested spike |
|---|---|---|
| WiFi UDP audio streaming | Can the Pico W transmit 384 bytes/ms over UDP without dropping? | Send UDP datagrams in a tight loop; measure throughput with `iperf` or packet capture |
| 96 kHz / 24-bit audio | Does `snd-usb-audio` accept UAC2 at 96 kHz over Full Speed? | Change sample rate in descriptors and `quisk_conf`; check `arecord -l` reports 96000 |
| Music player / DAC | Can you write a WAV header parser and feed a DAC from microSD? | Parse a WAV header from a known file on the SD card; print sample rate and bit depth |
| Standalone receiver (no PC) | Can you render a waterfall on the OLED with sufficient speed? | Compute a 64-point FFT and display magnitude bars on OLED at ≥5 Hz update rate |
| Standalone receiver (no PC) | I2S DAC output at 48 kHz | Output a 1 kHz sine wave via the DAC via PIO I2S TX; confirm on oscilloscope |

Run your spike. If it works: great — continue building. If it fails: document why and pivot to a modified plan.

> **Deliverable:** `lab8/spike_result.md` — what you tried, how you did it, and what the result was. If the spike failed, include your revised plan.

---

## Part 5 — Catch-Up Track (if Lab 7 not working)

If you are using today to finish Phase 1, work through this checklist with instructor support:

- [ ] UAC2 enumerates (Lab 6 verified) — if not, fix descriptors first
- [ ] `arecord` captures audio (even if noisy) — if not, debug TinyUSB audio TX callback
- [ ] Quisk waterfall shows something — if not, check `name_of_sound_capt` in config
- [ ] CDC tune still works — if not, check interface ordering in composite descriptor
- [ ] `phase1-complete` tag created and pushed

When Lab 7 is working, start Part 1 of this lab with the remaining time.

---

### Commit Checklist

By end of today's lab (or by Tuesday May 26 at 2 p.m. at the latest):

- [ ] `phase1-complete` git tag pushed to GitHub
- [ ] Extension branch `extension/<short-name>` created and pushed
- [ ] `lab8/EXTENSION_PLAN.md` — project plan with milestones
- [ ] `lab8/spike_result.md` — risk spike result (or catch-up progress notes)
- [ ] `lab8/lab8_report.md` — Phase 1 status answers from Part 1, hardware checkout notes
- [ ] Any hardware checked out from instructor logged in the sign-out sheet
