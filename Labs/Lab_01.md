# CPTR 480 — Lab 1
## Hardware Bring-Up: 2026 Board First Light
**Date:** Tuesday, March 31, 2026 | 2:00–5:00 p.m.
**Due:** All deliverables committed to your GitHub repo by **2 p.m., Tuesday April 7**

---

> **Note — Pico Debug Stack:** Debug Stacks are not arriving until Week 2. Debug Stack soldering, firmware flashing, and logic-analyzer measurements have been moved to **Lab 2**.
>
> **Soldering is still required today:** The YD-RP2040 ships without headers. You will solder the pin headers to it before any firmware work can begin. Hardware bugs discovered during bring-up may also require touch-up soldering — this is normal for an initial board bring-up.

---

### Learning Objectives
By the end of this lab you will be able to:
- Solder pin headers to a PCB and verify the result
- Flash MicroPython onto a Raspberry Pi Pico and run scripts with MicroPico in VS Code
- Measure DC power-supply rails with a multimeter on a live PCB
- Write and run a MicroPython I2C scan and drive an OLED display
- Control GPIO-driven LEDs, a WS2812B NeoPixel, and read GPIO inputs with MicroPython
- Debug first-article hardware faults using a multimeter, schematic inspection, and systematic reasoning
- Begin a professional board README, populating it with real measured data

---

### What You Need
- WWU 2026 Intro to CAD Experimenter's Board with YD-RP2040 (headers not yet soldered)
- Soldering iron, solder, flux
- **USB-C cable** (the YD-RP2040 uses USB-C, not the Micro-USB of the official Pico)
- Multimeter
- Your laptop with VS Code installed (Copilot free tier must be working, and you must have finished installing the toolchain from Assignment 1)
- **KiCad 10 installed** with the 2026 board schematic (`Intro-to-CAD-2026/Intro-to-CAD-2026.kicad_sch`) already confirmed to open — you will need it to trace GPIO pin numbers ([frohro/Intro-to-CAD-2026](https://github.com/frohro/Intro-to-CAD-2026), cloned as a submodule)

---

> ### ⚠️ YD-RP2040 vs Official Raspberry Pi Pico — Critical Differences
>
> The board in your kit uses the **YD-RP2040 v1.3**, a Chinese enhanced Pico clone. Before you write any firmware, be aware of these differences (all are called out in the [YD-RP2040_V1.3 schematic](../YD-RP2040_V1.3/YD-RP2040_V1.3.kicad_sch)):
>
> | Feature | Official Pico | YD-RP2040 v1.3 |
> |---------|--------------|----------------|
> | USB connector | Micro-USB | **USB-C** |
> | Pinout | Standard Pico pinout | **Different — verify from YD-RP2040 schematic** |
> | Onboard user LED | GPIO25, simple GPIO | **WS2812B NeoPixel RGB on GPIO23 — needs NeoPixel protocol** |
> | GPIO25 | Drives built-in LED + SMPS mode | No visible LED; acts as standard GPIO |
> | Onboard user button | None | **User switch on GPIO24** |
> | VSYS pin (pin 39) | Can be used as 5 V power input | **Output only — do NOT apply voltage here** |
> | Voltage regulator | RT6150 switching (≈600 mA) | **ME6211C LDO (300 mA max) — tighter current budget** |
> | Castellated holes | Yes | **No** — cannot be soldered flat to a PCB |
> | VBUS/power diode | Standard Schottky | Diode added to prevent USB/external power conflict |
> | GPIO23 | RT6150 power-save control | **WS2812B NeoPixel data** |
> | Flash | 2 MiB Winbond | 2 MiB Winbond (same size) |
>
> **The most common mistake:** copying a standard Pico blink example that uses `machine.Pin(25, machine.Pin.OUT).toggle()` and wondering why nothing lights up. On the YD-RP2040, GPIO25 is not connected to a visible LED. Use the NeoPixel on GPIO23, or use one of the 2026 board's four GPIO-driven status LEDs.

---

## Part 0 — Solder the YD-RP2040 Headers (~30 min)

The YD-RP2040 ships without pin headers. Until they are soldered it cannot seat properly in the 2026 board's socket, and you cannot make electrical contact with any GPIO.

1. Inspect the YD-RP2040. Locate the two rows of through-holes along each long edge (standard 2.54 mm pitch).
2. Insert the supplied pin headers (long pins down, short pins up) and seat the board in the 2026 board socket to hold alignment while you solder.
3. Solder every pin. Work carefully — cold joints are a common embedded debugging problem. Ask your instructor to inspect your solder joints before proceeding.
4. With headers soldered, re-seat the YD-RP2040 in the 2026 board socket and plug in the USB-C cable. The board should power on (check the power LED if present, or measure 3.3 V on a rail).

> **Hardware bug note:** If something on the 2026 board doesn't work later in the lab, the root cause is often a solder joint or a missing/wrong component — not firmware. Don't hesitate to reach for the iron and multimeter. Touch-up soldering during bring-up is expected and normal.

> **Deliverable:** Take a photo of your completed solder joints. Commit it as `lab1/yd_rp2040_headers_photo.jpg`.

---

## Part 1 — Flash MicroPython and Blink the 4 LEDs (~25 min)

The 2026 board has four status LEDs that are driven serially (chained). Your first firmware goal is to blink all four.

1. Download the latest **MicroPython preview build** (`.uf2`) for the RP2040/Pico from [micropython.org/download/RPI_PICO](https://micropython.org/download/RPI_PICO/). Scroll down to the **Releases** section and download the most recent **preview** `.uf2` (not the stable release) — Lab 2 requires features only present in the preview builds, such as `machine.I2S` with MCK support.
2. Hold the BOOTSEL button on the YD-RP2040 while plugging in the USB-C cable. A mass-storage drive called `RPI-RP2` appears. Drag and drop the MicroPython `.uf2` file onto it. The board reboots into MicroPython.
3. In VS Code, install the **MicroPico** extension (search "MicroPico" in the Extensions panel). Connect to the board via MicroPico.
4. Use GitHub Copilot Chat to generate a MicroPython script that blinks the four status LEDs on the **2026 board** (these are ordinary GPIO-driven LEDs on the 2026 board PCB — not the YD-RP2040's built-in NeoPixel). Prompt example:
   > *"Write a MicroPython script for a Raspberry Pi Pico that blinks GPIOs [X, Y, Z, W] at 2 Hz. The LEDs should blink together."*
   
   **You will need to determine the correct GPIO numbers from the 2026 board schematic.** Open `Intro-to-CAD-2026/Intro-to-CAD-2026.kicad_sch` (from [github.com/frohro/Intro-to-CAD-2026](https://github.com/frohro/Intro-to-CAD-2026)), find the LED net labels, and trace them to the Pico/YD-RP2040 GPIO pins. This is the first schematic-reading exercise.

   > **Note:** Do NOT try `machine.Pin(25, machine.Pin.OUT).toggle()` as a quick test — on the YD-RP2040, GPIO25 is not connected to a visible LED (unlike the official Pico where GPIO25 drives the built-in green LED). The YD-RP2040's built-in user LED is a **WS2812B NeoPixel RGB LED** on **GPIO23**, which requires the NeoPixel protocol rather than simple GPIO writes. Stick to the 2026 board's four status LEDs for this exercise.

5. Run the script. All four LEDs should blink. Adjust the script if they don't.

> **Deliverable:** Save your script as `lab1/blink_leds.py`. It must include a comment at the top listing which GPIO pin maps to which LED.

---

## Part 2 — Power Rail Validation (~25 min)

The 2026 board has several power supply rails (3.3 V digital, 3.3 VA analog, possibly others — check the schematic and your Assignment 1 notes).

> **YD-RP2040 power notes:**
> - The YD-RP2040 uses a **ME6211C LDO** (linear regulator, 300 mA max) to produce 3.3 V from USB VBUS. This is a lower current budget than the official Pico's RT6150 switching regulator. Keep total 3.3 V load (MCU + all peripherals on the 2026 board) well under 300 mA.
> - **Do not apply voltage to the VSYS pin (pin 39).** On the official Pico, VSYS is a power input/output. On the YD-RP2040, it is output-only from the regulator — driving it externally will fight the LDO and may damage the board.
> - The YD-RP2040 has a **BAT54 Schottky diode** to prevent conflict between USB power and any external "Vin" supply, so you can safely power the board from either USB-C or an external 5 V on Vin — but not simultaneously through VSYS.

For each rail:
1. Identify the test point or a convenient measurement node (bypass capacitor leg, connector pin).
2. Measure with a multimeter referenced to GND.
3. Record in `lab1/power_rails.md` using this table:

| Rail | Expected Voltage | Measured Voltage | Pass/Fail |
|------|-----------------|-----------------|-----------|
| 3.3 V digital (from YD-RP2040 LDO) | 3.3 V ±3% | | |
| VDDA (analog rail, from VDDA schematic) | TBD from schematic | | |
| VDD (any additional rails) | TBD | | |

> **Caution:** If any rail is significantly out of spec (>5% error) or missing, stop and ask the instructor before proceeding.

> **Deliverable:** `lab1/power_rails.md` with the completed table.

---

## Part 3 — I2C Scan + OLED Display (~25 min)

1. Use Copilot to generate a MicroPython script that scans the I2C bus(es) on the 2026 board:
   ```python
   import machine
   i2c = machine.I2C(0, scl=machine.Pin(X), sda=machine.Pin(Y))
   print(i2c.scan())
   ```
   **Again, find the correct I2C GPIO pins and bus number from the schematic.**

2. Run the scan. You should see at least the **Si5351a** (address `0x60`) and the **OLED controller** (typically `0x3C` or `0x3D`). Record all addresses found.

3. Use Copilot to generate a MicroPython script that displays "Hello CPTR480" on the OLED. Install `ssd1306.py` (MicroPython driver) via MicroPico's package manager, or ask Copilot to generate a minimal framebuffer write.

4. Take a photo of the OLED displaying your message.

> **Deliverable:** Save scripts as `lab1/i2c_scan.py` and `lab1/oled_hello.py`. Save the photo as `lab1/oled_hello.jpg`. In `lab1/measurements.md`, add a table of I2C addresses found and what device each corresponds to.

---

## Part 4 — RGB LED + Button and Encoder (~20 min)

The 2026 board has two more peripherals you can test without the Debug Stack.

### 4a — WS2812B NeoPixel (GPIO 22)

MicroPython's built-in `neopixel` module requires no extra install. Use Copilot to generate a script that cycles the NeoPixel through at least four colors (e.g., red → green → blue → white), with 600 ms per color. Prompt suggestion:
> *"Write a MicroPython script that cycles a single WS2812B NeoPixel on GPIO 22 through red, green, blue, and white at 600 ms per color, using the built-in neopixel module."*

1. Generate the script with Copilot, then upload and run it.
2. Verify the colors light as expected.
3. Take a photo with at least two distinct colors visible.

> **Deliverable:** Photo saved as `lab1/rgb_led_photo.jpg`.

### 4b — Push Button + Rotary Encoder

| Component | Signal | GPIO |
|-----------|--------|------|
| Push button | BTN | GPIO 21 |
| Encoder center click | SW_C | GPIO 5 |
| Encoder quadrature A | ANG_A | GPIO 18 |
| Encoder quadrature B | ANG_B | GPIO 17 |

All inputs use internal pull-ups (active-low when pressed). Use Copilot to generate a MicroPython script that decodes quadrature on ANG_A/ANG_B and prints an incrementing/decrementing counter, and prints a message when BTN or SW_C is pressed. Press **Ctrl-C** to stop. Prompt suggestion:
> *"Write a MicroPython script for an RP2040 that reads a quadrature rotary encoder on GPIO 18 (A) and GPIO 17 (B), printing a counter on each detent. Also detect a push button on GPIO 21 and a center click on GPIO 5, printing a message when each is pressed. Use internal pull-ups."*

1. Generate the script with Copilot, then upload and run it.
2. Turn the encoder CW and CCW — verify the counter increments and decrements.
3. Press the push button and the encoder center click — verify both are detected.
4. Take a screenshot of the MicroPico terminal output.

> **Deliverable:** Screenshot of console output saved as `lab1/encoder_console.png`. Record pass/fail in `lab1/measurements.md`.

---

## Part 5 — Start the 2026 Board README (~20 min)

Good embedded documentation starts the moment you touch the hardware, not after everything works. Using your `assignment1/2026_board_README_draft.md` skeleton as a starting point, create the real `2026_board_README.md` at the **root of your repo** and populate it with what you know right now.

Fill in at minimum:

1. **Overview** — one paragraph: what the board is, who designed it, what it is for.
2. **Hardware Modules table** — list every module on the board; mark each one with your status from today:
   - ✅ Verified in Lab 1
   - ⚠️ Bug found — see Known Issues
   - 🔲 Not yet tested (will be Lab 2)
3. **Pin Assignments table** — transfer the GPIO → signal mapping you traced from the schematic and confirmed today (LEDs, I2C bus, encoder, button, NeoPixel at minimum).
4. **Power Supply Rails table** — fill in the measured values from Part 2.
5. **Known Issues / Bugs Found** — this section is required, not optional. For every hardware problem encountered today, record:
   - What symptom was observed (e.g., "LED3 did not light")
   - What the root cause was (e.g., "cold solder joint on R12", "wrong component value", "net connected to wrong GPIO")
   - What was done to fix or work around it (e.g., "reflowed joint — now working", "bridged with wire — pending board revision")
   - Whether the fix was confirmed working

   If no bugs were found, write "No issues found during Lab 1 bring-up" — but be honest. First-article boards almost always have something.

Leave sections that require Lab 2 data (op-amp bias, logic analyzer captures, Si5351a, DAC, etc.) with `TBD — see Lab 2`. This document will be completed and finalised after Lab 2.

> **Deliverable:** `2026_board_README.md` at the repo root, committed with today's partial data. The **Known Issues / Bugs Found** section must be present and filled in honestly.

Before midnight, your GitHub repo must contain:

- [ ] `lab1/yd_rp2040_headers_photo.jpg` — YD-RP2040 solder joint photo
- [ ] `lab1/blink_leds.py` — 4-LED MicroPython blink with GPIO pin comments
- [ ] `lab1/power_rails.md` — DC rail measurements table
- [ ] `lab1/i2c_scan.py` — I2C scan script
- [ ] `lab1/oled_hello.py` — OLED display script
- [ ] `lab1/oled_hello.jpg` — photo of OLED working
- [ ] `lab1/rgb_led_photo.jpg` — NeoPixel photo (at least two distinct colors)
- [ ] `lab1/encoder_console.png` — terminal screenshot showing encoder and button events
- [ ] `lab1/measurements.md` — LED GPIO map, I2C address table, encoder pass/fail
- [ ] `2026_board_README.md` — started at repo root, with overview, partial module table, GPIO table, power rail measurements, and known issues
- [ ] `.gitignore` — excludes `build/` directories

---

## AI Reflection (add to `journal/week1.md`)
Write 1–2 paragraphs (informal, graded on completion not quality):
- What did you ask Copilot? What prompts worked well? What did it get wrong?
- What would have been harder or slower without AI assistance?
