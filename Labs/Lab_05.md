# CPTR 480 — Lab 5
## USB CDC Enumeration + Quisk Hardware Driver + Live Demo
**Date:** Tuesday, April 28, 2026 | 2:00–5:00 p.m.
**Due:** All deliverables committed to your GitHub repo by **2 p.m., Tuesday May 5**

---

### Learning Objectives
By the end of this lab you will be able to:
- Enumerate a USB CDC device using TinyUSB on the RP2040
- Define a simple ASCII serial protocol for LO frequency control
- Write a Quisk hardware Python module that opens a CDC serial port and calls `ChangeFrequency()` to dispatch tuning commands to your firmware
- Run a live Quisk SDR session using your CDC firmware + a borrowed sound card

---

### What You Need
- 2026 board with YD-RP2040
- Pico Debug Stack
- Si5351a driver from Lab 3
- TinyUSB (included in the Pico SDK — no separate install needed)
- USB-C data cable (must support data, not charge-only)
- A host PC running Linux
- Quisk source already in the course repo at `../../quisk/` (run with `python3 quisk.py`)
- `python3-serial` package on the host (`sudo apt install python3-serial`)
- A borrowed USB sound card (from instructor) for the live Quisk demo
- Your Assignment 4 USB notes

---

### Overview

This lab has two products that wire together at the end:

**Product 1 (firmware):** Your YD-RP2040 enumerates as a CDC ACM device. It parses a simple ASCII line protocol — you define it — and tunes the Si5351a accordingly.

**Product 2 (Python):** A small Quisk hardware module (`quisk_hardware_wwusdr.py`) that inherits from Quisk's `Hardware` base class, opens your CDC serial port with `pyserial`, and sends a tune command every time Quisk calls `ChangeFrequency()`.

Together they replace a conventional SDR's hardware driver. The borrowed USB sound card provides I/Q audio; your firmware controls the LO. You will see a live waterfall.

Seeing the waterfall at Lab 5 is the motivational payoff that makes Labs 6 and 7 concrete: your next job is to replace that borrowed sound card with UAC2 audio streaming from your own firmware.

---

## Part 1 — TinyUSB CDC Project Setup (~20 min)

1. Create `lab5/` with a `CMakeLists.txt`. Include all three source files, the include path so TinyUSB can find `tusb_config.h`, and — critically — disable the SDK's built-in USB stdio:
   ```cmake
   add_executable(lab5 main.c si5351.c usb_descriptors.c)

   # tusb_config.h is in the project directory — add it to the include path.
   # Without this, TinyUSB cannot find your configuration header.
   target_include_directories(lab5 PRIVATE .)

   target_link_libraries(lab5 pico_stdlib tinyusb_device tinyusb_board hardware_i2c)

   # MUST be 0 when using TinyUSB directly.
   # pico_enable_stdio_usb internally registers its own TinyUSB application;
   # enabling it alongside your own tusb_init() call causes duplicate descriptor
   # callbacks and enumeration failure.  All output goes through tud_cdc_write_str().
   pico_enable_stdio_usb(lab5 0)
   pico_enable_stdio_uart(lab5 0)
   pico_add_extra_outputs(lab5)
   ```
   Also copy `pico_sdk_import.cmake` from Lab 3 or Lab 4 into `lab5/`, and copy `si5351.h` and `si5351.c` from Lab 3.

2. Copy `tusb_config.h` from the TinyUSB CDC example at
   `$PICO_SDK_PATH/lib/tinyusb/examples/device/cdc_msc/` into `lab5/`. Adjust:
   - `CFG_TUD_CDC 1` (one CDC interface)
   - `CFG_TUD_MSC 0`
   - `CFG_TUD_AUDIO 0`

3. Create `usb_descriptors.c` with a device descriptor (choose your own vendor/product strings), configuration descriptor, CDC interface descriptor, and notification + data endpoints.

4. Confirm the project builds before adding any application logic.

> **Copilot use:** Copilot generates TinyUSB CDC boilerplate reliably. Verify that `bcdUSB` is `0x0200`, `bDeviceClass` is `0xEF` (Miscellaneous, for composite), and an IAD is present — even for a CDC-only device. Missing the IAD causes Windows enumeration failures.

---

## Part 2 — CDC Enumeration Verification (~20 min)

1. Flash your firmware. On the Linux host:
   ```bash
   lsusb
   ls /dev/ttyACM*
   ```
   Your device and a `/dev/ttyACM0` (or similar) should appear.

2. Open a serial terminal and confirm echo:
   ```bash
   minicom -D /dev/ttyACM0 -b 115200
   ```
   Add a simple echo-back in your CDC receive callback (`"Echo: " + received line`) and confirm it works.

3. Capture `lsusb -v` for your device and save as `lab5/lsusb_output.txt`. You will compare this against Lab 6's UAC2 output.

> **Deliverable:** `lab5/lsusb_output.txt` showing your CDC ACM device.

---

## Part 3 — Define a Protocol and Implement the Firmware Parser (~35 min)

You control both ends of this link, so you design the protocol. Keep it simple:

| Direction | Command | Meaning |
|---|---|---|
| Host → firmware | `F<hz>\n` | Set CLK0+CLK1 of Si5351a to `<hz>` Hz (both I and Q channels) |
| Host → firmware | `?\n` | Query current frequency |
| Firmware → host | `F<actual_hz>\n` | Actual frequency set after integer-grid snap |
| Firmware → host | `RANGE\n` | Requested frequency outside [1 MHz, 160 MHz] |
| Firmware → host | `ERR\n` | Unrecognised command |

Example: `F7100000\n` requests 7.1 MHz; the firmware replies `F7099733\n` (the nearest achievable integer-grid frequency, N=26 M=90).

Implement a line-accumulator in your CDC task:

```c
static char line_buf[32];
static int  line_len = 0;

void cdc_task(void) {
    if (!tud_cdc_available()) return;
    while (tud_cdc_available()) {
        char c = (char)tud_cdc_read_char();
        if (c == '\n' || c == '\r' || line_len >= (int)sizeof(line_buf) - 1) {
            line_buf[line_len] = '\0';
            if (line_len > 0) handle_line(line_buf);   /* skip blank lines */
            line_len = 0;
        } else {
            line_buf[line_len++] = c;
        }
    }
}

void handle_line(const char *line) {
    if (line[0] == 'F') {
        uint32_t hz = (uint32_t)strtoul(line + 1, NULL, 10);
        if (hz >= 1000000 && hz <= 160000000) {
            /* si5351_set_freq_integer() returns the actual frequency set
             * after snapping to the nearest integer (N, even-M) grid point.
             * A return value of 0 means no valid pair was found. */
            uint32_t actual = si5351_set_freq_integer(i2c0, hz);
            if (actual > 0) {
                current_hz = actual;
                char resp[24];
                snprintf(resp, sizeof(resp), "F%lu\n", (unsigned long)actual);
                tud_cdc_write_str(resp);
            } else {
                tud_cdc_write_str("RANGE\n");
            }
        } else {
            tud_cdc_write_str("RANGE\n");
        }
        tud_cdc_write_flush();
    } else if (line[0] == '?') {
        char resp[24];
        snprintf(resp, sizeof(resp), "F%lu\n", (unsigned long)current_hz);
        tud_cdc_write_str(resp);
        tud_cdc_write_flush();
    } else {
        tud_cdc_write_str("ERR\n");
        tud_cdc_write_flush();
    }
}
```

Test manually from minicom: send `F7100000` (press Enter), confirm the Si5351a CLK0 changes (logic analyzer or spectrum analyzer).

> **Deliverable:** `lab5/tune_demo.txt` — terminal session showing tune commands and acknowledgements.

---

## Part 4 — Write the Quisk Hardware Module (~45 min)

Quisk does **not** have a built-in serial frequency protocol. Quisk calls `ChangeFrequency(tune, vfo)` in a Python hardware class that *you write*, which then sends whatever protocol you chose to your firmware via pyserial. This is the same pattern used by every custom Quisk radio (see `quisk_conf_openradio.py` in the Quisk source for a clear example).

Create `lab5/quisk_hardware_wwusdr.py`:

```python
# Quisk hardware file for WWU 2026 SDR board (Lab 5: CDC LO control)
import serial
from quisk_hardware_model import Hardware as BaseHardware

SERIAL_PORT = "/dev/ttyACM0"   # change if yours differs
BAUD        = 115200

class Hardware(BaseHardware):
    def open(self):
        try:
            self.ser = serial.Serial(SERIAL_PORT, BAUD, timeout=0.1)
            return "WWU SDR: CDC port %s opened." % SERIAL_PORT
        except Exception as e:
            self.ser = None
            return "WWU SDR: could not open %s: %s" % (SERIAL_PORT, e)

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()

    def ChangeFrequency(self, tune, vfo, source='', band='', event=None):
        # vfo is the LO centre frequency (Hz); tune is the demodulation
        # cursor offset within the audio window — pass through unchanged.
        # Read back F<actual_hz>\n so Quisk's display reflects the
        # integer-grid snap done in firmware.
        if self.ser and self.ser.is_open:
            try:
                self.ser.reset_input_buffer()
                self.ser.write(("F%d\n" % vfo).encode())
                resp = self.ser.readline().decode(errors='replace').strip()
                if resp.startswith('F'):
                    vfo = int(resp[1:])
            except Exception:
                self.ser = None
        return tune, vfo
```

Create `lab5/quisk_conf_wwusdr.py`:

```python
# Quisk config for WWU 2026 SDR — Lab 5 (borrowed sound card)
import sys, os
sys.path.insert(0, os.path.dirname(__file__))   # so Quisk finds our hardware file

hardware_file_name = "quisk_hardware_wwusdr.py"

name_of_sound_capt = "alsa:USB Audio"   # fragment of borrowed sound card ALSA name
name_of_sound_play = "pulse"
sample_rate        = 48000
channel_i          = 0
channel_q          = 1
```

> **Finding the borrowed sound card's ALSA name:** run `arecord -l` on the host to list capture devices and find the USB sound card. Use any fragment of its description as `name_of_sound_capt`.

---

## Part 5 — Live Quisk Demo (~40 min)

**Hardware setup:**
1. Plug the borrowed USB sound card into the host PC.
2. Connect the 2026 board's QSD I/Q audio output to the borrowed sound card's line input (ask instructor for the patch cable).
3. Your YD-RP2040 is connected over USB-C (CDC enumerated as `/dev/ttyACM0`).

**Running Quisk:**
```bash
cd ../../quisk          # the course-repo quisk/ directory
python3 quisk.py --config ../2026/CPTR480_Template/lab5/quisk_conf_wwusdr.py
```

In the Quisk Config → Radios screen, add a radio of type **ConfigFileRadio** pointing to your `.py` config, or pass `--config` on the command line.

You should see a moving waterfall. Clicking the frequency display or tuning with the mouse calls `ChangeFrequency()` → your hardware file sends `F<hz>\n` to the RP2040 → Si5351a LO changes → the SDR receives a different slice of spectrum.

> **What you are seeing:** The borrowed sound card provides I/Q audio; your CDC firmware controls the LO. Labs 6 and 7 eliminate the borrowed sound card by streaming I/Q audio over UAC2 from your own firmware.

> **Deliverable:** Screenshot `lab5/quisk_demo.png` — Quisk waterfall with a frequency displayed, showing a live signal (or at minimum a noise floor that moves when you change frequency).

---

## Part 6 — Robustness (~15 min)

Before Lab 6, harden both sides:

**Firmware:**
1. Bad command (no `F` prefix, non-numeric): respond `ERR\n`, do not crash.
2. Frequency out of [1 MHz, 160 MHz]: respond `RANGE\n`, do not call `si5351_set_freq`.
3. USB disconnect/reconnect: firmware should re-enumerate without reboot. Test by unplugging and replugging while running.

**Quisk hardware file:**
1. Wrap `self.ser.write(...)` in a `try/except` — if the CDC port disappears (unplugged board), catch the exception and set `self.ser = None` rather than crashing Quisk.

Document any issues in `lab5/lab5_report.md`.

---

### Commit Checklist

By 2:00 p.m., Tuesday May 5:

- [ ] `lab5/CMakeLists.txt`
- [ ] `lab5/pico_sdk_import.cmake` — copy from Lab 3 or Lab 4
- [ ] `lab5/tusb_config.h`
- [ ] `lab5/usb_descriptors.c`
- [ ] `lab5/si5351.h` — copy from Lab 3
- [ ] `lab5/si5351.c` — copy from Lab 3
- [ ] `lab5/main.c` — TinyUSB CDC task loop + tune command parser
- [ ] `lab5/quisk_hardware_wwusdr.py` — Quisk hardware module
- [ ] `lab5/quisk_conf_wwusdr.py` — Quisk config file
- [ ] `lab5/lsusb_output.txt` — host-side CDC enumeration
- [ ] `lab5/tune_demo.txt` — terminal session showing tune commands working
- [ ] `lab5/quisk_demo.png` — Quisk waterfall screenshot
- [ ] `lab5/lab5_report.md` — robustness testing results and anomalies
