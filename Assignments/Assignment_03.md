# CPTR 480 — Assignment 3
## Si5351a Driver Analysis + DMA Preparation
**Assigned:** Monday, April 13 | **Due:** Monday, April 20 at 11:00 a.m.

---

### Overview
This assignment bridges the Si5351a lab work into preparing for audio DMA. It has three parts: reviewing and documenting your Si5351a driver based on Lab 3 results, deepening your understanding of DMA chaining for audio, and your weekly AI reflection journal.

> *Specific questions and deliverable details will be finalized and posted to the course repo by Wednesday April 15.*

---

### Part 1 — Si5351a Driver Review

Based on your Lab 3 work:

1. Document the I2C register writes your driver makes to configure a given output frequency. What register sequence produces a **7.168 MHz** LO output (integer $N=28$, $M=96$, crystal 24.576 MHz)? Capture the I2C traffic with the logic analyzer and include the annotated screenshot.
2. Why is 7.168 MHz achievable with integer dividers but 7.1 MHz is not? Show that $7{,}100{,}000 / 24{,}576{,}000$ is irreducible and explain what this means for the Si5351a PLL.
3. What is the minimum and maximum frequency your driver can produce on CLK0 in integer mode, given the 2026 board's 24.576 MHz reference crystal and the VCO range 600–900 MHz? Show the calculation.
4. How does integer-only PLL/MS mode reduce phase jitter compared to fractional mode? Why does this matter for SDR receive performance (reciprocal mixing)?

> **Deliverable:** `assignment3/si5351_notes.md` with diagram/screenshot committed.

---

### Part 2 — DMA Chaining for Audio

Read RP2040 Datasheet §2.5 (DMA), focusing on ping-pong / chaining patterns. Answer in `assignment3/dma_notes.md`:

- Draw (ASCII or embedded image) the buffer layout for a DMA ping-pong scheme capturing 48 kHz stereo 32-bit audio. Label buffer A, buffer B, the DMA channel, and where the interrupt fires.
- At 48 kHz stereo 32-bit, how many bytes per second does the DMA need to transfer? How many bytes per DMA interrupt if you use 4ms half-buffers?
- What happens if the CPU doesn't process the filled buffer before the DMA wraps around? How would you detect this in firmware?

> **Deliverable:** `assignment3/dma_notes.md` committed.

---

### Part 3 — AI Reflection Journal (Week 3)

In `journal/week3.md`, add your weekly reflection (1–2 paragraphs):
- What did you ask Copilot this week (lab + homework)?
- One moment where Copilot's suggestion was wrong or misleading for embedded work specifically — what was it and why was it wrong?
- One thing you learned from reading source or datasheet that Copilot got approximately right but not exactly right.

---

### Commit Checklist

By Monday April 20 at 11:00 a.m.:

- [ ] `assignment3/si5351_notes.md` with logic analyzer screenshot
- [ ] `assignment3/dma_notes.md` with buffer diagram and calculations
- [ ] `journal/week3.md`
