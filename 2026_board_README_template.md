# WWU 2026 SDR Board

**Designed by:** Dr. Frohne + WWU Intro to CAD Lab, 2025–2026
**Board revision:** TBD
**Status:** ⚠️ Bring-up in progress — last updated [DATE] by [YOUR NAME]

---

## Overview

[1–2 sentences: What does this board do? Who is it for?]

---

## Hardware Modules

| Module | IC / Component | Interface | GPIO / Pins | Status |
|--------|---------------|-----------|-------------|--------|
| Microcontroller | Raspberry Pi Pico (RP2040) | — | — | ✅ Verified |
| Audio ADC | PCM1808 (24-bit stereo) | I2S | TBD | 🔲 Not yet |
| Audio DAC | TBD | I2S / I2C | TBD | 🔲 Not yet |
| Clock Generator | Si5351a | I2C | TBD | 🔲 Not yet |
| SDR Front-End | QSD (Quadrature Sampling Detector) | Passive / clocked by Si5351a | — | 🔲 Not yet |
| OLED Display | TBD (SSD1306 or similar) | I2C / SPI | TBD | 🔲 Not yet |
| MicroSD | — | SPI | TBD | 🔲 Not yet |
| H-Bridge | TC118S or similar | GPIO | TBD | 🔲 Not yet |
| CMOS Oscillator | TBD | — (output to Si5351a XTAL) | TBD | 🔲 Not yet |
| Status LEDs (×4) | — | GPIO | TBD | 🔲 Not yet |
| RF Input | Amphenol RF connector | — | — | 🔲 Not yet |

*Status key: ✅ Verified | ⚠️ Partial | ❌ Fault found | 🔲 Not yet tested*

---

## Pin Assignments

| Signal | Pico GPIO | Direction | Notes |
|--------|-----------|-----------|-------|
| I2C SDA | TBD | Bidirectional | Shared bus — Si5351a + OLED |
| I2C SCL | TBD | Output | |
| I2S BCLK | TBD | Output from Pico | PCM1808 bit clock |
| I2S LRCLK | TBD | Output from Pico | PCM1808 word select |
| I2S DATA | TBD | Input to Pico | PCM1808 serial data |
| SPI SCK | TBD | Output | MicroSD |
| SPI MOSI | TBD | Output | MicroSD |
| SPI MISO | TBD | Input | MicroSD |
| SPI CS (SD) | TBD | Output | MicroSD chip select |
| LED 1 | TBD | Output | |
| LED 2 | TBD | Output | |
| LED 3 | TBD | Output | |
| LED 4 | TBD | Output | |
| H-Bridge IN1 | TBD | Output | |
| H-Bridge IN2 | TBD | Output | |

*Fill in from the 2026 board schematic. Trace each net from the Pico connector sheet.*

---

## Power Supply Rails

| Rail | Nominal Voltage | Source | Measured Voltage | Pass? |
|------|----------------|--------|-----------------|-------|
| 3.3V Digital | 3.3V | Pico onboard SMPS | TBD | — |
| VDDA (Analog) | TBD V | VDDA regulator circuit | TBD | — |
| [Additional rails] | TBD | TBD | TBD | — |

---

## Op-Amp Bias Points

| Op-Amp | Location | Output Pin | Expected DC (V) | Measured DC (V) | Saturated? |
|--------|----------|------------|----------------|----------------|------------|
| | | | | | |

---

## Bring-Up Checklist

Complete in order. Do not proceed to the next step until the current one passes.

- [ ] **Visual inspection** — no solder bridges, all components present and oriented correctly
- [ ] **3.3V digital rail within ±5%** — measured: ___V
- [ ] **VDDA rail within ±5%** — measured: ___V
- [ ] **Op-amps not saturated** — all outputs within expected range
- [ ] **CMOS oscillator running** — measured frequency: ___Hz (expected: ___Hz)
- [ ] **Si5351a I2C responsive at 0x60** — confirmed by `i2c.scan()` result
- [ ] **Si5351a produces commanded frequency** — logic analyzer verified at ___Hz
- [ ] **OLED displays text** — confirmed working
- [ ] **MicroSD mounts FAT32 filesystem** — file write/read verified
- [ ] **H-bridge responds to GPIO** — logic analyzer verified on control pins
- [ ] **Debug Probe (Yapicoprobe) SWD connection** — VS Code debugger breakpoint confirmed
- [ ] **PCM1808 audio capture** — (Week 5)
- [ ] **USB enumeration as CDC + UAC2** — (Week 7)

---

## Known Issues

| Issue | Severity | Workaround | Status |
|-------|----------|------------|--------|
| | | | |

---

## Firmware Notes

- [Add notes that future firmware developers need to know, e.g., power-on sequence constraints,
  clock configuration order, I2C address conflicts, etc.]

---

## Tools Used for Bring-Up

- MicroPython + MicroPico VS Code extension
- Dr. Gusman's Logic Analyzer (on Pico Debug Stack, U1)
- Yapicoprobe SWD debug probe (on Pico Debug Stack, U2)
- Fluke multimeter

---

## References

- [2026 Board Schematic (KiCad)](../Intro-to-CAD-2026/Intro-to-CAD-2026.kicad_sch)
- [PCM1808 Datasheet](https://www.ti.com/product/PCM1808)
- [Si5351a Datasheet](https://www.silabs.com/documents/public/data-sheets/Si5351-B.pdf)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Pico Debug Stack README](../Pico_Debug_Stack/README.md)
