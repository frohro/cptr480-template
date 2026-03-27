# CPTR 480 — Lab 2
## Full Board Validation + 2026 Board README
**Date:** Tuesday, April 7, 2026 | 2:00–5:00 p.m.
**Due:** All deliverables committed to your GitHub repo by **2 p.m., Tuesday April 14**

---

### Learning Objectives
By the end of this lab you will be able to:
- Systematically validate embedded hardware modules using a multimeter, logic analyzer, and MicroPython scripts
- Measure DC operating points and identify op-amp saturation
- Verify I2C and SPI peripheral communication with a logic analyzer
- Document hardware findings in a professional embedded board README

---

### What You Need
- WWU 2026 SDR board + Pico (already flashed with MicroPython from Lab 1)
- **Pico Debug Stack** (hopefully arrives this week)
- Laptop
- Soldering iron, solder, flux
- USB-C cable (for the YD-RP2040) + USB micro/USB-A cable (for the Debug Stack)
- Multimeter
- Oscilloscope or logic analyzer
- VS Code + MicroPico (MicroPython still on the Pico)
- Your `assignment1/2026_board_README_draft.md` — you should have the skeleton ready
- Signal generator (optional, for QSD RF input test)

> **Running scripts with MicroPico:** Open any `.py` file in VS Code, then click the **Run** button in the MicroPico status bar (bottom of the window), or use the command palette → *MicroPico: Run current file on Pico*. You do **not** need `mpremote`.

---

### Overview

This lab has two phases. **Phase 1** sets up the Pico Debug Stack — the soldering and firmware work deferred from Lab 1. **Phase 2** is a systematic hardware validation pass over every module on the 2026 board **except** the PCM1808 I2S audio interface (that comes in Lab 4). For each module in Phase 2, you will:
1. Write a short MicroPython test script (use Copilot to generate it quickly)
2. Measure operating points with a multimeter and/or logic analyzer
3. Record pass/fail and measured values
4. Populate your 2026 board README with the findings

Keep moving — 3 hours is tight for this many tasks. Write your results in `lab2/validation_report.md` as you go. Power rail measurements were recorded in Lab 1; copy those values into `validation_report.md` at the top.

---

## Part 1 — Pico Debug Stack Bring-Up (~75 min)

The Pico Debug Stack contains a debug probe (Yapicoprobe, U2) and a logic analyzer (Dr. Gusman's LogicAnalyzer, U1). Do this first so the logic analyzer is available for the rest of lab.

### 1a — Solder the Debug Stack (~40 min)

1. Inspect the board. Locate U1, U2, and the GPIO header pads. Consult the Debug Stack README (in the repo) for which headers need to be soldered.
2. Solder the pin headers. Work carefully — cold joints are a common embedded debugging problem. Ask your instructor to inspect your solder joints before proceeding.
3. Plug the Debug Stack into a USB port. On Linux, run:
   ```bash
   lsusb
   ```
   You should see a USB hub (GL850G) and at least one RP2350B device appear. If you see them, your soldering is good.

> **Deliverable:** Take a photo of your completed solder joints. Commit it as `lab2/debug_stack_photo.jpg`.

### 1b — Build and Flash the Logic Analyzer Firmware (U1) (~15 min)

The Logic Analyzer firmware lives in the `pico-debug-stack` branch of the [Pico-Debug-Stack repository](https://github.com/frohro/Pico_Debug_Stack). It is a customized build of Dr. Gusman's LogicAnalyzer tuned for the Debug Stack hardware.

1. Clone the repository and check out the branch:
   ```bash
   git clone https://github.com/frohro/Pico_Debug_Stack.git
   cd Pico_Debug_Stack
   git checkout pico-debug-stack
   ```
2. Build the Logic Analyzer firmware:
   ```bash
   cd logicanalyzer/Firmware/LogicAnalyzer_V2
   mkdir build && cd build
   cmake .. -DPICO_BOARD=pico2
   make -j4
   ```
   A successful build produces a `.uf2` file.
3. Enter BOOTSEL mode on **U1**: hold U1's BOOTSEL button while pressing RESET (or while plugging in the Debug Stack).
4. Drag-and-drop the `.uf2` onto U1's drive. U1 reboots into Logic Analyzer mode.

### 1c — Build and Flash Yapicoprobe (U2) (~15 min)

[Yapicoprobe](https://github.com/rgrr/yapicoprobe) is an open-source CMSIS-DAP/picoprobe-compatible debug probe firmware.

1. Clone and build Yapicoprobe:
   ```bash
   git clone https://github.com/rgrr/yapicoprobe.git
   cd yapicoprobe
   mkdir build && cd build
   cmake .. -DPICO_BOARD=pico2
   make -j4
   ```
2. Enter BOOTSEL mode on **U2** and flash the resulting `.uf2` the same way as U1.
3. After flashing, run `lsusb` — you should now see both a CMSIS-DAP device (U2) and the Logic Analyzer (U1) listed through the Debug Stack's USB hub.

### 1d — Measure the LED Blink with the Logic Analyzer (~10 min)

Verify the logic analyzer is working by measuring the blink from Lab 1.

1. Install the **Logic Analyzer host software** from the [LogicAnalyzer GitHub releases page](https://github.com/gusmanb/logicanalyzer/releases).
2. With the Lab 1 `blink_leds.py` script still on the Pico, connect a Debug Stack probe pin to the GPIO driving one of the four status LEDs.
3. Open the Logic Analyzer software, capture the blink, and measure the **on-time**, **off-time**, and **period**. Verify they match the `time.sleep()` delay in your script.

> **Deliverable:** Save the screenshot as `lab2/logic_analyzer_blink.png`. In `lab2/validation_report.md`, record: GPIO pin measured, expected period, measured period, pass/fail.

---

## Part 2 — Op-Amp Bias Point Check (~20 min)

The 2026 board has op-amps in the signal chain for the SDR and audio paths. At rest (no signal input), op-amp outputs should sit at their designed DC bias point — typically mid-rail (≈ VDD/2) for AC-coupled audio stages.

1. Identify the op-amp ICs on the board from the schematic.
2. Measure the DC voltage at each op-amp output pin with a multimeter.
3. Compare to expected bias from the schematic (look for the feedback network resistor values to calculate the expected bias).
4. **Saturation** = output rail (near 0V or near VDD) when it shouldn't be. This indicates a wiring fault, power issue, or wrong component.

Record in `lab2/validation_report.md`:

| Op-Amp | Pin | Expected DC bias | Measured | Saturated? |
|--------|-----|-----------------|----------|------------|
| | | | | |

---

## Part 3 — CMOS Oscillator (~10 min)

The 2026 board has a CMOS crystal oscillator that provides a reference clock.

1. Find the oscillator output pin from the schematic.
2. Probe it with the logic analyzer (or oscilloscope if available).
3. Verify:
   - Frequency matches the oscillator's rated frequency (from the component value in the schematic)
   - Signal is a clean square wave (no ringing beyond one period, reasonable duty cycle)

> **Deliverable:** Save a logic analyzer screenshot as `lab2/osc_capture.png`. Record frequency in `lab2/validation_report.md`.

---

## Part 4 — Si5351a Clock Generator (~30 min)

The Si5351a is on the I2C bus: **SDA = GPIO12, SCL = GPIO13**, I2C address `0x60`.

You will need two files: a small driver library (`si5351.py`) and a test script (`si5351_test.py`). Use Copilot to generate both. A good prompt:

> *"Write a MicroPython Si5351a driver for the RP2040 that initialises the chip over I2C at address 0x60, configures PLLA, and sets CLK0 to a given frequency in Hz. Then write a test script that scans the I2C bus, initialises the Si5351a, sets CLK0 to 10 MHz, checks PLLA_LOL and PLLB_LOL status bits, prints the result, and then sweeps through 1 MHz, 7.1 MHz, 14.2 MHz, and 28 MHz, skipping any frequency that can't be generated cleanly with integer dividers from a 25 MHz crystal."*

**Step-by-step:**

1. Upload `si5351.py` to the Pico via MicroPico (*Upload file to Pico* in the file explorer). This installs the driver so MicroPython can import it.
2. Upload and run `si5351_test.py`.
3. You should see output confirming the device was found at `0x60`, PLLA locked, and CLK0 set to 10.000 MHz.
4. Verify CLK0 on the logic analyzer or oscilloscope — you should see a clean 10 MHz square wave.
5. Capture one of the successful sweep frequencies.

> **Note:** `si5351.py` is the driver you will reuse in Lab 5 when tuning the Si5351a for the SDR local oscillator. Keep it on the Pico.

> **Deliverable:** Commit `lab2/si5351.py` and `lab2/si5351_test.py`. Save a logic analyzer (or oscilloscope) screenshot as `lab2/si5351_capture.png`. Record PLLA_LOL status and measured CLK0 frequency in `lab2/validation_report.md`.

---

## Part 5 — CJC4334H DAC Audio Output (~20 min)

The 2026 board has a **CJC4334H** 24-bit I2S DAC connected to the Pico on:

| Signal | GPIO |
|--------|------|
| SCLK (I2S bit clock) | GPIO6 |
| LRCK (I2S word clock) | GPIO7 |
| MCLK (master clock)  | GPIO8 |
| SDIN (serial data IN to DAC) | GPIO9 |

The CJC4334H requires **no register writes** — it auto-detects its operating mode from the MCLK/LRCK ratio. At 48 kHz with MCLK = 256 × 48 000 = **12.288 MHz**, the chip enters standard I²S mode automatically.

MicroPython's `machine.I2S` generates the MCLK automatically when you pass `mck=Pin(8)` — no PWM tricks needed (requires MicroPython ≥ 1.20 on RP2040).

Use Copilot to generate `dac_test.py`. A good prompt:

> *"Write a MicroPython script for the RP2040 that uses machine.I2S to stream a continuous 440 Hz sine wave to a CJC4334H DAC. SCLK = GPIO6, LRCK = GPIO7, MCLK = GPIO8, SDIN = GPIO9. Sample rate 48000 Hz, 16-bit stereo. Generate the sine wave into a buffer and play it in a loop."*

> **Audio output caution:** AOUTL and AOUTR are line-level outputs (~2.8 Vpp full scale). Use high-impedance headphones (≥ 32 Ω) or connect to a line-input amplifier. Do not drive low-impedance earbuds directly.

**Steps:**
1. Run `dac_test.py` on the Pico.
2. Attach a logic analyzer to GPIO6, GPIO7, GPIO9. Verify:
   - SCLK: **3.072 MHz**
   - LRCK: **48 kHz**
   - SDIN: valid I2S frames
3. Connect headphones or a speaker amplifier to the DAC output. You should hear a 440 Hz tone.
4. Press **Ctrl-C** in the MicroPico terminal to stop.

> **Deliverable:** Save script as `lab2/dac_test.py`. Save logic analyzer screenshot as `lab2/dac_i2s_capture.png`. Record I2S clock frequencies and audio result in `lab2/validation_report.md`.

---

## Part 6 — OLED Display (~15 min)

(Quick verification — you did this in Lab 1, but now verify it in the context of the full system with the Debug Stack connected.)

Display the text: `CPTR480 Lab2` on line 1 and `Board OK` on line 2.

> **Deliverable:** Photo saved as `lab2/oled_lab2.jpg`.

---

## Part 7 — MicroSD Card (~25 min) ⚠️ Optional

> **You must bring your own FAT32-formatted microSD card.** The lab does not supply cards. If you do not have one, note "skipped — no card" in `validation_report.md` and move on — this part is optional for the Lab 2 grade.

> **Card format:** The `sdcard` module only supports **FAT32**. exFAT and NTFS will fail to mount.
> - Linux: `mkdosfs -F 32 /dev/sdX`
> - Windows: right-click → Format → FAT32
> - macOS: Disk Utility → MS-DOS (FAT)

SPI pin assignments on the 2026 board:

| Signal  | GPIO |
|---------|------|
| MISO    | GPIO0 |
| CS      | GPIO1 |
| SPI_CK  | GPIO2 |
| MOSI    | GPIO3 |
| SD_CD   | GPIO4 (card detect, active-low) |

Use Copilot to generate `microsd_test.py`. A good prompt:

> *"Write a MicroPython script for the RP2040 that mounts a FAT32 microSD card over SPI with MISO=GPIO0, CS=GPIO1, SCK=GPIO2, MOSI=GPIO3, card-detect=GPIO4. List the root directory, write 'CPTR480 test' to cptr480_test.txt, read it back and verify the contents, and handle OSError gracefully if no card is present."*

> **Deliverable:** Save script as `lab2/microsd_test.py`. In `lab2/validation_report.md`, record: mount successful (Y/N), file write/read successful (Y/N). If skipped, record reason.

---

## Part 8 — H-Bridge (~15 min)

The H-bridge (TC118S) accepts GPIO control signals. Control pins on the 2026 board: **INA = GPIO27, INB = GPIO28**.

Use Copilot to generate `hbridge_test.py`. A good prompt:

> *"Write a MicroPython script for the RP2040 that drives an H-bridge (TC118S) on INA=GPIO27 and INB=GPIO28. Cycle through Coast (0,0), Forward (1,0), Coast, Reverse (0,1), Coast, Brake (1,1), Coast, pausing 1 second in each state, and print each state to the console."*

Truth table for reference:

| INA | INB | Mode |
|-----|-----|------|
| 0 | 0 | Coast (high-Z) |
| 1 | 0 | Forward |
| 0 | 1 | Reverse |
| 1 | 1 | Brake |

1. Attach a logic analyzer to GPIO27 and GPIO28 before running.
2. Run the script, capture the seven-state sequence.

> **Deliverable:** Save script as `lab2/hbridge_test.py`. Logic analyzer screenshot as `lab2/hbridge_capture.png`.

---

## Part 9 — QSD SDR Front-End DC Check (~15 min)

The QSD front-end is a passive mixer driven by four-phase clocks from the Si5351a. At this stage, without an RF input, you can only verify DC operating points.

1. With the Si5351a running (from Part 4), measure DC voltages at the mixer output nodes using a multimeter.
2. Verify there are no unexpected DC shorts to ground or rail.
3. If a signal generator is available, inject a small RF tone near 7 MHz (40m band) into the RF input. With the Si5351a tuned to 4× that frequency (28 MHz), you should see an audio-frequency output — but this is a stretch goal for today.

> **Deliverable:** Record DC measurements in `lab2/validation_report.md`.

---

## Part 10 — Complete the 2026 Board README (~30 min)

Using your Assignment 1 skeleton and the measured data from Parts 1–11, finalize `2026_board_README.md` at the root of your repo. This is the document the next person who picks up this board will read.

It must include:
- **Overview:** What the board does, who designed it, when
- **Hardware Modules table:** Every module, its IC, its interface, and its status (verified / not yet verified)
- **Pin Assignment table:** Signal → Pico GPIO, substantially complete from schematic
- **Power Supply Rail table:** With your measured values from Part 1
- **Op-amp bias summary:** From Part 2
- **Bring-Up Checklist:** Each item checked off with your measured values
- **Known Issues:** Anything unexpected found today — even minor
- **Tools Used:** What MicroPython libraries, logic analyzer configuration, etc.

> **This is a graded deliverable.** A future student or engineer should be able to pick up this README and reproduce your validation work.

---

## Commit Checklist

## Commit Checklist

By midnight, Tuesday April 8:

- [ ] `lab2/debug_stack_photo.jpg` — solder joint photo
- [ ] `lab2/logic_analyzer_blink.png` — logic analyzer blink capture (from Part 1d)
- [ ] `lab2/validation_report.md` — all module pass/fail with measured values (include power rail rows from Lab 1)
- [ ] `lab2/si5351.py` + `lab2/si5351_test.py` + `lab2/si5351_capture.png` — CLK0 frequency confirmed, PLLA locked
- [ ] `lab2/osc_capture.png`
- [ ] `lab2/dac_test.py` + `lab2/dac_i2s_capture.png`
- [ ] `lab2/oled_lab2.jpg`
- [ ] `lab2/microsd_test.py` *(or note "skipped — no card")*
- [ ] `lab2/hbridge_test.py` + `lab2/hbridge_capture.png`
- [ ] `2026_board_README.md` — complete, at repo root

---

## AI Reflection (add to `journal/week2.md`)
Write 1–2 paragraphs:
- How did Copilot help you work faster today? Where did it give you wrong information you had to correct?
- What is one thing you understand about the 2026 board now that you did not understand before this lab?
