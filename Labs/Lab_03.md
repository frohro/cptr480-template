# CPTR 480 — Lab 3
## Si5351a I2C Driver in C
**Date:** Tuesday, April 14, 2026 | 2:00–5:00 p.m.
**Due:** All deliverables committed to your GitHub repo by **2 p.m., Tuesday April 21**

---

### Learning Objectives
By the end of this lab you will be able to:
- Write and verify bare-metal I2C transactions in C using the Pico SDK
- Read and write Si5351a registers and confirm results with the logic analyzer
- Set an LO output frequency using the Si5351a PLL and multisynth divider chain
- Use the SWD debug probe to inspect I2C register state at runtime
- Structure a reusable C driver with a clean API

---

### What You Need
- 2026 board with YD-RP2040
- Pico Debug Stack (Yapicoprobe + Logic Analyzer)
- VS Code with Cortex-Debug extension configured (SWD probe working from Lab 1)
- Pico SDK C toolchain working
- Si5351a datasheet and AN619 register map (download from [Silicon Labs](https://www.silabs.com/documents/public/data-sheets/Si5351-B.pdf))
- Your Assignment 3 notes (Si5351a register sequence and frequency calculations)

> **Schematic note:** Before writing any code, confirm the crystal frequency and I2C GPIO pins from the 2026 board schematic. The crystal is **not** 25 MHz; the schematic label is the authoritative value.

---

### Overview

This is your first significant C embedded driver. You will write a Si5351a driver from scratch (with Copilot assistance), verify it with the logic analyzer, and confirm frequency output with the logic analyzer or oscilloscope.

The Si5351a produces the local oscillator (LO) for the QSD SDR front end. If the LO frequency is wrong, the SDR will receive on the wrong frequency. This driver is Phase 1 critical path — it will be used in every subsequent lab.

**Transition:** You are moving from MicroPython (Labs 1–2) to the Pico C/C++ SDK. The SDK abstracts hardware to a degree, but you are now writing compiled code with real-time constraints.

---

## Part 1 — Project Setup (~15 min)

1. In your team repo, create the directory structure:
   ```
   lab3/
     CMakeLists.txt
     si5351.h
     si5351.c
     main.c
   ```

2. Copy the top-level `CMakeLists.txt` pattern from `lab1/` and adapt it for `lab3/`. Add `pico_enable_stdio_usb(lab3 1)` so you can see `printf` output over USB serial.

3. Add `hardware_i2c` to `target_link_libraries` — the Pico SDK I2C HAL.

4. Verify the project builds (empty `main.c` returning 0 is fine at this point) before writing driver code.

> **Copilot use:** Use Copilot to generate the CMakeLists.txt skeleton. Verify it against the Pico SDK documentation — Copilot occasionally uses deprecated function names.

---

## Part 2 — I2C Initialization and Bus Scan (~20 min)

1. In `main.c`, initialize I2C on the correct Pico GPIO pins for the 2026 board. Determine the correct pins from the 2026 board schematic (the Si5351a SDA/SCL lines map to specific Pico GPIOs — check the schematic, don't guess).

2. Set I2C clock to 400 kHz (fast mode).

3. Write an I2C bus scan that probes all 128 addresses and prints which ones respond over USB serial. The Si5351a should appear at address `0x60`.  (See the datasheet.)

4. Capture the I2C scan transactions with the logic analyzer. Save the capture as `lab3/i2c_scan_capture.png`.

> **Deliverable:** Screenshot of I2C scan showing address `0x60` responding.

---

## Part 3 — Si5351a Driver: Register Read/Write (~45 min)

Create `si5351.h` and `si5351.c` with at minimum:

```c
bool si5351_init(i2c_inst_t *i2c);
void si5351_write_reg(i2c_inst_t *i2c, uint8_t reg, uint8_t val);
uint8_t si5351_read_reg(i2c_inst_t *i2c, uint8_t reg);
```

1. Implement `si5351_write_reg` and `si5351_read_reg` using `i2c_write_blocking` and `i2c_read_blocking` from the Pico SDK.

2. Implement `si5351_init`:
   - Disable all outputs (register 3, all bits set)
   - Set internal load capacitance (register 183, crystal load = 10 pF for the 2026 board — confirm from schematic)
   - Wait for SYS_INIT (register 0, **bit 7**) to clear — this indicates the device has finished its internal startup sequence

3. Read back register 0 after init and print its value to confirm SYS_INIT clears (device is ready).

4. Capture the init I2C traffic with the logic analyzer. Annotate the capture (in your validation report) identifying at minimum the write to register 3 and register 183.

> **Copilot use:** Copilot can generate the register write/read functions accurately. Be cautious with the init sequence — Copilot sometimes omits the SYS_INIT poll or uses wrong default values. Cross-check against the Si5351a datasheet §3.

---

## Part 4 — Frequency Output (~60 min)

Implement frequency setting. The Si5351a frequency architecture:

$$f_{out} = \frac{f_{xtal} \times N}{M \times R}$$

Where $N$ is the integer PLL multiplier and $M$ is the integer multisynth divider. The reference XTAL on the 2026 board is **24.576 MHz** (verify from the schematic — this is not 25 MHz).

### Why integer-only mode?

The Si5351a can also operate in fractional PLL mode, but for an SDR LO, **integer mode is strongly preferred**:

- In fractional mode, the PLL continuously dithers between two integer states to approximate a non-integer ratio. This dithering creates periodic phase jitter on the LO output.
- Phase jitter on the LO causes **reciprocal mixing**: a strong nearby signal folds noise into the passband and masks weak signals you are trying to receive.
- In integer mode ($N$ and $M$ both integers), the PLL locks to a single stable state. No dithering, no periodic jitter.

The 24.576 MHz crystal was deliberately chosen: $24{,}576{,}000 = 512 \times 48{,}000$, an exact multiple of the PCM1808 sample rate. This makes it possible to find integer $(N, M)$ pairs at useful 40-meter frequencies.

### Why 7.1 MHz is not achievable in integer mode

Try it:
$$7{,}100{,}000 = \frac{24{,}576{,}000 \times N}{M}$$
$$\frac{N}{M} = \frac{7{,}100{,}000}{24{,}576{,}000} = \frac{1775}{6144}$$
These are coprime — there is no small-integer solution.
This is worth discovering during the lab: **pick a frequency, check whether it is achievable with integer dividers, and understand why the crystal choice matters.**

### Why M must be even in integer mode

AN619 §4.1 states explicitly: *"When MSx\_INT is set, the value of `a` must be even."* This constraint applies to the **output** multisynths MS0–MS5 only. The PLL feedback dividers (MSNA, MSNB) may use any integer — so N=27 or N=29 are perfectly valid.

**Why does this matter for the QSD?** The 2026 board drives the I and Q mixer switches directly from **two Si5351a outputs**: CLK0 → I_CLK and CLK1 → Q_CLK (verified from the schematic). The Si5351a generates the 90° quadrature internally using its PHOFF register — there is no external ÷4 flip-flop chain. Both CLK0 and CLK1 must have a 50% duty cycle to ensure balanced charge injection into the switching mixers.

If M is odd, the integer-mode output divider produces an asymmetric waveform: one half-cycle spans $\lceil M/2 \rceil$ VCO periods and the other spans $\lfloor M/2 \rfloor$. The resulting duty cycle error creates even-harmonic mixing products and unequal I/Q gain, directly degrading image rejection. Even M guarantees both half-cycles are equal.

### How the PHOFF register produces exactly 90°

The PHOFF register (AN619 Table 4) shifts an output in units of $T_{VCO}/4$. For a 90° offset between CLK0 (I) and CLK1 (Q), the required delay is one quarter of the output period:

$$\text{delay} = \frac{T_{out}}{4} = \frac{M \cdot T_{VCO}}{4}$$

Setting this equal to $\text{PHOFF} \times T_{VCO}/4$:

$$\text{PHOFF} = M$$

For any even integer M in the 40 m band (M = 90–100), PHOFF = M fits in the 7-bit register and gives **exactly** 90°, with no fractional remainder. This is the same integer exactness that makes integer PLL mode attractive — the quadrature path is also free of approximation.

With CLK1_PHOFF = M: CLK1 (Q) is delayed by 90° relative to CLK0 (I), giving I = cos(ωt) and Q = sin(ωt). Whether the SDR displays upper or lower sideband by default depends on the QSD mixer topology; Quisk can swap I/Q in software.

### Implementation

Add to your driver:

```c
// Returns the actual frequency achieved (exact for integer pairs).
// Returns 0 if no valid integer (N, M) pair exists in the VCO range.
uint32_t si5351_set_freq_integer(i2c_inst_t *i2c, uint32_t freq_hz);
```

The implementation should:
1. Search for integer $N$ (range 25–36, giving VCO 614–885 MHz) and **even** integer $M$ (≤ 127) such that $N \times 24{,}576{,}000 / M$ is closest to `freq_hz`. Odd M is invalid when `MSx_INT=1` (AN619 §4.1 — 50% duty cycle requirement for QSD switching mixers).
2. For both PLL and MS in integer mode: $P_1 = 128N - 512$, $P_2 = 0$, $P_3 = 1$
3. Write PLL A registers (MSNA, regs 26–33)
4. Write MS0 registers (42–49) for CLK0 (I channel)
5. Write MS1 registers (50–57) for CLK1 (Q channel) — **same $M$ value as MS0**; both outputs share PLL A
6. Set CLK0 control register (reg 16): `MS0_INT=1` (bit 6), `MS_SRC=PLLA` (bits 5:4 = 00), `CLK_SRC=MS` (bits 2:0 = 011)
7. Set CLK1 control register (reg 17): same bit pattern as reg 16
8. Set CLK1_PHOFF = M in register 166 to produce a 90° delay on Q relative to I (CLK0_PHOFF register 165 stays at 0)
9. Apply PLLA soft reset only (register 177, write `0x20` — bit 5 resets PLLA; do not write `0xA0` which also resets PLLB)
10. Enable CLK0 and CLK1 outputs (register 3, clear bits 0 and 1)

**Test target:** Set CLK0 to **7.168 MHz** — achieved exactly with $N = 28$, $M = 96$:
$$\frac{24{,}576{,}000 \times 28}{96} = \frac{688{,}128{,}000}{96} = 7{,}168{,}000 \text{ Hz} \checkmark$$
This lands in the 40-meter SSB segment. When Quisk samples at 48 kHz, it displays 7.144–7.192 MHz — a useful slice of the band.

Verify with the logic analyzer that:
- The CLK0 output toggles at the correct frequency (measure period, calculate frequency)
- The I2C writes match your calculated register values

> **Deliverable:** Logic analyzer capture of CLK0 output at 7.168 MHz saved as `lab3/si5351_7168khz.png`. Calculated vs. measured frequency and integer $(N, M)$ pair documented in `lab3/lab3_report.md`.

---

## Part 5 — SWD Debug Verification (~30 min)

1. In VS Code with Cortex-Debug, set a breakpoint after `si5351_set_freq`.
2. Use the debug watch panel to inspect:
   - The calculated PLL multiplier and MS divider values
   - The raw register bytes being written
3. Step through one I2C write transaction and watch the I2C hardware FIFO register change in the Peripherals view.

This is the core embedded debugging skill — verifying hardware register state without printf.

> **Deliverable:** Screenshot of VS Code Cortex-Debug with at least one peripheral register visible saved as `lab3/swd_debug.png`.

---

## Part 6 — Tune an Integer-Grid Range (~20 min)

Add a loop to your `main.c` that steps CLK0 through five integer-achievable frequencies across the 40-meter band, with a 500 ms delay between each:

| Frequency (Hz) | N  | M   | VCO (MHz) | M even? |
|---------------|----|----|----------|---------|
| 7,059,064     | 27 | 94  | 663.552  | ✓ |
| 7,099,733     | 26 | 90  | 638.976  | ✓ |
| 7,127,040     | 29 | 100 | 712.704  | ✓ |
| **7,168,000** | **28** | **96** | **688.128** | ✓ |
| 7,212,522     | 27 | 92  | 663.552  | ✓ |

> **Note:** A naive search that allows odd M would suggest N=28/M=97 ≈ 7.094 MHz here, but M=97 is odd and the output would have a non-50% duty cycle. The correct nearest choice is N=26/M=90 ≈ 7.100 MHz.

Verify with the logic analyzer that each frequency is distinct and monotonically increasing.

Notice that the steps are **irregular** — this is a fundamental property of integer-mode tuning. The accessible LO frequencies form a sparse grid determined by the crystal frequency. For continuous tuning, the software (Quisk) moves the demodulation cursor within the ±24 kHz display window; only when the cursor moves outside that window does the firmware retune the LO to the next integer-grid frequency.

This confirms your frequency calculation is correct across a range, not just at one point.

---

### Commit Checklist

By 2:00 p.m., Tuesday April 21:

- [ ] `lab3/CMakeLists.txt` — builds cleanly
- [ ] `lab3/si5351.h` and `lab3/si5351.c` — driver with init, register read/write, frequency setting
- [ ] `lab3/main.c` — I2C scan, init, frequency test, range sweep
- [ ] `lab3/i2c_scan_capture.png` — logic analyzer showing address 0x60
- [ ] `lab3/si5351_7168khz.png` — CLK0 at 7.168 MHz (N=28, M=96)
- [ ] `lab3/swd_debug.png` — Cortex-Debug peripheral view
- [ ] `lab3/lab3_report.md` — integer (N, M) pairs, calculated vs. measured frequencies, crystal frequency from schematic
