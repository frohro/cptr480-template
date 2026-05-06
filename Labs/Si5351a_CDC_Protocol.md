# Si5351a CDC Control Protocol — Student Reference
## CPTR 480 / Lab 5

---

## Overview

For this lab, Quisk (running on the Linux host) does all of the frequency-selection math and sends the Pico pre-computed Si5351a register values over USB CDC (virtual serial port).

Your Pico firmware's only job is to:

1. Parse the incoming command.
2. Write the supplied integers directly into the Si5351a registers.
3. Reply with the required status string.

You do **not** compute PLL integers in the Pico. You do **not** search for a better LO. You just parse and write.

---

## Transport

| Parameter | Value |
|-----------|-------|
| Interface | USB CDC (virtual serial port, `/dev/ttyACM0` or `/dev/ttyACM1`) |
| Baud rate | 115200 (baud rate is ignored for CDC — the USB pipe is full speed, but set 115200 in any terminal program) |
| Line ending | Each command ends with `\n`. Each response line ends with `\n`. |
| Encoding | ASCII |
| Duplex | Half — host sends a command, Pico replies before the host sends the next |

All commands and responses are plain ASCII text, one logical message per line.

---

## Startup Handshake

When Quisk opens the serial port it:

1. Sends `0x03` (Ctrl-C) then `0x04` (Ctrl-D). Your firmware must silently discard these characters.
2. Waits up to 6 seconds for a line containing the text `SDR ready`.
3. Proceeds to the `VER`, `XTAL`, and `RATE` setup commands.

**Your firmware must print a line that contains `SDR ready` during its startup** before entering the main command loop.

---

## Command Reference

Every command is a single ASCII line terminated with `\n`.  
Every response is one or two ASCII lines, each terminated with `\n`.

---

### `VER` — Query firmware version

**Sent by host:**
```
VER
```

**Expected reply (two lines):**
```
VER,<version_string>
OK
```

Example:
```
VER,SDR Pi Pico version 0.1
OK
```

---

### `XTAL` — Query crystal reference frequency

Quisk needs the exact reference oscillator frequency of your board so it can compute PLL integers on the host side.

**Sent by host:**
```
XTAL
```

**Expected reply (two lines):**
```
XTAL,<freq_hz>
OK
```

`<freq_hz>` is a floating-point number in Hz. For the 2026 board:

```
XTAL,24576000.0
OK
```

---

### `MODE` — Query hardware mixer topology

Quisk needs to know whether the board uses a **direct** mixer (Si5351a CLK0/CLK1 drive the mixer directly at the LO frequency with a 90° phase offset between channels) or a **Johnson-counter** topology (Si5351a CLK0 feeds a 4-bit Johnson-counter ring divider that produces four 90°-separated clocks at ¼ the input frequency).

**Sent by host:**
```
MODE
```

**Expected reply (two lines):**
```
MODE,<topology>
OK
```

`<topology>` is one of:

| Value | Meaning |
|-------|---------|
| `DIRECT` | No divider. CLK0/CLK1 drive the mixer at the LO frequency. N must be **even**; CLK1 phase-offset register provides 90° quadrature. |
| `JOHNSON` | 4-bit Johnson counter after CLK0. Si5351a outputs 4 × LO frequency. N may be **any integer** (odd or even); quadrature is provided by the counter. |

Example:
```
MODE,JOHNSON
OK
```

> **Firmware default:** If your firmware does not implement the `MODE` command, Quisk falls back to `DIRECT` mode. Implementing `MODE` correctly allows Quisk to compute the right Si5351a integers for your hardware.

---

### `RATE,<hz>` — Set audio sample rate

Quisk tells the Pico the sound-card sample rate so the firmware can store it for any bandwidth-related decisions. For this lab, just store the value and acknowledge.

**Sent by host:**
```
RATE,48000
```

**Expected reply:**
```
OK
```

---

### `FREQ,` — Query current LO frequency and status

A bare `FREQ,` (with nothing after the comma) asks the Pico to report what it most recently programmed.

**Sent by host:**
```
FREQ,
```

**Expected reply (two lines):**
```
<last_hz>
OK,<type>,<signed_offset>
```

If no frequency has been programmed yet, you can reply with a default frequency and `OK,X,0`.

---

### `FREQ,<hz>,<N>,<a>,<b>,<c>,<P1>,<P2>,<P3>` — Program the LO

This is the main tuning command. Quisk has already done all of the math. It sends you the exact integers you need.

**Sent by host:**
```
FREQ,<hz>,<N>,<a>,<b>,<c>,<P1>,<P2>,<P3>
```

| Field | Type | Meaning |
|-------|------|---------|
| `hz` | integer | Si5351a CLK0 output frequency in Hz. In `DIRECT` mode this equals the logical LO frequency. In `JOHNSON` mode this equals 4 × the logical LO frequency. |
| `N` | integer | Integer Multisynth output divider for CLK0 (and CLK1 in `DIRECT` mode). **Even** in `DIRECT` mode; **any integer** (odd or even) in `JOHNSON` mode. |
| `a` | integer | Integer part of PLL feedback multiplier $M = a + b/c$ |
| `b` | integer | Numerator of fractional part of $M$ |
| `c` | integer | Denominator of fractional part of $M$ (max 1,048,575) |
| `P1` | integer | PLLA register parameter 1 (see math below) |
| `P2` | integer | PLLA register parameter 2 |
| `P3` | integer | PLLA register parameter 3 |

**Expected reply (two lines):**
```
<hz>
OK,<type>,<signed_offset>
```

- The first line echoes back the integer `hz` value that was requested.
- The second line is the status:

| Response | When to use |
|----------|-------------|
| `OK,G,<signed_offset>` | Integer PLL (b == 0). `signed_offset` = actual Si5351a CLK0 output − requested `hz`, in Hz |
| `OK,F,0` | Fractional PLL (b != 0, exact frequency) |
| `OK,X,0` | Fallback / out-of-spec VCO |

For integer mode (`b == 0`), `signed_offset` is the Hz difference between the **actual Si5351a CLK0 output** and the `hz` value in the command. In `DIRECT` mode this equals the logical LO error; in `JOHNSON` mode it is 4× the logical LO error. For fractional mode the offset is zero by definition, so always reply `OK,F,0`.

**Example exchange for 7.074 MHz:**
```
host → FREQ,7074000,100,28,31250,15625,3072,0,15625
pico → 7074000
pico → OK,F,0
```

---

## The Si5351a Math (for your understanding)

Quisk computes these values on the host. Your firmware receives them already computed and writes them directly to the chip. This section explains what they mean so you understand what you are writing.

### Two-stage frequency synthesis

$$F_{out} = \frac{F_{xtal} \times M}{N}$$

where

$$M = a + \frac{b}{c}$$

- $F_{xtal}$ is your board's reference crystal frequency (e.g., 24,576,000 Hz).
- $M$ is the PLL A feedback multiplier (programs the VCO frequency).
- $N$ is the integer Multisynth output divider (divides the VCO to get $F_{out}$).

### Constraints enforced by Quisk

| Constraint | `DIRECT` mode | `JOHNSON` mode |
|------------|---------------|----------------|
| Si5351a CLK0 output | = logical LO | = 4 × logical LO |
| VCO range | 600 MHz ≤ $F_{xtal} \times M$ ≤ 900 MHz | same |
| $M$ range | $14 < M < 91$ | same |
| $N$ | Even integer, 6–126 | Any integer, 4–127 |
| $c$ | $1 \leq c \leq 1{,}048{,}575$ | same |
| Quadrature source | CLK1 phase-offset register (Reg 166 = N) | Johnson counter (Reg 166 not written) |

### Register formulas

These are how `P1`, `P2`, and `P3` are derived from `a`, `b`, and `c`:

$$P1 = 128 \times a + \left\lfloor \frac{128 \times b}{c} \right\rfloor - 512$$

$$P2 = 128 \times b - c \times \left\lfloor \frac{128 \times b}{c} \right\rfloor$$

$$P3 = c$$

When $b = 0$ (integer PLL, called "golden" mode), $P2 = 0$ and $P3 = 1$.

### Multisynth output divider registers

For both CLK0 and CLK1, the Multisynth registers use a fixed integer $N$:

$$\text{MS\_P1} = 128 \times N - 512, \quad \text{MS\_P2} = 0, \quad \text{MS\_P3} = 1$$

---

## What Your Pico Firmware Must Do

After parsing a valid `FREQ,...` command, program the Si5351a in this order:

### Step 1 — Set CLK0 and CLK1 to integer mode, source = PLLA

For each of CLK0 (control register 16) and CLK1 (control register 17):
- Set the PLL source bit to select PLLA.
- Set bit 6 (integer-mode bit) so the output Multisynth runs in integer mode.

### Step 2 — Program MS0 and MS1 with integer divider N

Compute:

```
MS_P1 = 128 * N - 512
MS_P2 = 0
MS_P3 = 1
```

Write those three values into both Multisynth 0 (base register 42) and Multisynth 1 (base register 50) using the standard 8-register Si5351 layout:

| Offset | Value |
|--------|-------|
| +0 | `(MS_P3 >> 8) & 0xFF` |
| +1 | `MS_P3 & 0xFF` |
| +2 | `(MS_P1 >> 16) & 0x03` |
| +3 | `(MS_P1 >> 8) & 0xFF` |
| +4 | `MS_P1 & 0xFF` |
| +5 | `((MS_P3 >> 12) & 0xF0) \| ((MS_P2 >> 16) & 0x0F)` |
| +6 | `(MS_P2 >> 8) & 0xFF` |
| +7 | `MS_P2 & 0xFF` |

### Step 3 — Write PLLA (Multisynth NA) with P1, P2, P3

PLLA base address is register 26. Write the same 8-register layout using the host-supplied `P1`, `P2`, `P3`:

| Register | Value |
|----------|-------|
| 26 | `(P3 >> 8) & 0xFF` |
| 27 | `P3 & 0xFF` |
| 28 | `(P1 >> 16) & 0x03` |
| 29 | `(P1 >> 8) & 0xFF` |
| 30 | `P1 & 0xFF` |
| 31 | `((P3 >> 12) & 0xF0) \| ((P2 >> 16) & 0x0F)` |
| 32 | `(P2 >> 8) & 0xFF` |
| 33 | `P2 & 0xFF` |

### Step 4 — Set the CLK1 phase offset for 90° quadrature (`DIRECT` mode only)

> **Skip this step if your board uses a Johnson counter (`JOHNSON` mode).** The Johnson counter generates the four 90°-separated phases internally; writing the phase-offset register would corrupt the phase relationship.

Write to register 166 (CLK1 Initial Phase Offset):

```
Reg[166] = N & 0x7F
```

**Why this gives exactly 90°:**  
The Si5351a interprets the PHOFF register in units of one quarter of the VCO period. Setting CLK1's offset to $N$ gives a delay of $N \times \frac{1}{4 \times F_{VCO}} = \frac{1}{4 \times F_{out}}$, which is exactly one quarter of the output period — 90°. This only works correctly when $N$ is a whole integer, which is exactly why both CLK0 and CLK1 are put in integer mode.

### Step 5 — Soft-reset PLLA

Write `0x20` to register 177 (PLL Reset register):

```
Reg[177] = 0x20   // bit 5 = reset PLLA
```

This forces CLK0 and CLK1 to re-lock in phase. Do this **after** all the register writes above; resetting before writing produces undefined output.

### Step 6 — Enable outputs

Write `0x00` to register 3 (Output Enable Control):

```
Reg[3] = 0x00   // all outputs enabled
```

### Step 7 — Send the reply

Print:

```
<hz>
OK,G,<signed_offset>      (if b == 0)
```
or
```
<hz>
OK,F,0                    (if b != 0)
```

---

## Complete Startup Sequence

```text
[Pico boots]
prints: "SDR ready"          ← Quisk waits for this line

host → VER
pico → VER,SDR Pi Pico version 0.1
pico → OK

host → XTAL
pico → XTAL,24576000.0
pico → OK

host → MODE
pico → MODE,JOHNSON          ← or MODE,DIRECT
pico → OK

host → RATE,48000
pico → OK

[Quisk is now operational; tuning begins — hz values below are Si5351a CLK0 output]

host → FREQ,7074000,100,28,31250,15625,3072,0,15625    ← DIRECT example
pico → 7074000
pico → OK,F,0

host → FREQ,28296000,24,28,0,1,3072,0,1                ← JOHNSON example (4 × 7.074 MHz)
pico → 28296000
pico → OK,G,0
```

---

## Control Character Handling

During startup, Quisk sends `0x03` (Ctrl-C) and `0x04` (Ctrl-D) to clear any previous REPL session that might be running. Your C firmware should discard these bytes silently — do not treat them as part of a command string.

---

## Division of Responsibility Summary

| Task | Where it runs |
|------|---------------|
| Choose where to place the LO relative to the audio passband | Quisk (host Python) |
| Search for integer vs. fractional PLL modes | Quisk (host Python) |
| Compute P1, P2, P3, N, a, b, c | Quisk (host Python) |
| Query crystal frequency to calibrate the search | Pico → Quisk |
| Parse the FREQ command | Pico (your C firmware) |
| Write Si5351a registers | Pico (your C firmware) |
| Set 90° phase offset on CLK1 | Pico (your C firmware) |
| Report programmed frequency and PLL type | Pico (your C firmware) |
