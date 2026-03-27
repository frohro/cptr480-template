# CPTR 480 — Assignment 2
## RP2040 Architecture Preparation + README Corrections
**Assigned:** Wednesday, April 8 | **Due:** Monday, April 13 at 11:00 a.m.

---

### Overview
Week 2's homework bridges the MicroPython validation work into the C/C++ SDK world starting in Week 3. It has two parts: correcting and completing the 2026 board README based on Lab 2 findings, and preparing for the DMA lab by reading the relevant RP2040 documentation.

---

### Part 1 — 2026 Board README Final Polish

Review your `2026_board_README.md` committed after Lab 2. Based on any corrections or additional findings:

1. Fix any errors discovered after the lab (wrong GPIO numbers, corrected voltage measurements, etc.).
2. Ensure every module in the Hardware Modules table has a "Status" entry: ✅ Verified / ⚠️ Partial / ❌ Not yet / 🔲 Not tested.
3. Add a **Firmware Notes** section with at least two bullet points about things firmware developers will need to know (e.g., "Si5351a must be configured before PCM1808 I2S clocks are stable" or "VDDA is sensitive to power-on sequence — see schematic sheet 3").

> **Deliverable:** Updated `2026_board_README.md` committed.

---

### Part 2 — RP2040 Architecture Reading

Read the following from the [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf) and answer the questions below in `assignment2/rp2040_notes.md`. Use Copilot to help you *understand* — but write answers in your own words.

**Section §2 (System Description):**
- What are the two Cortex-M0+ cores? What memory do they share?
- What is the bus fabric and why does it matter for real-time performance?
- What is the XIP (Execute-In-Place) flash interface, and why might it cause timing jitter?

**Section §2.5 (DMA):**
- How many DMA channels does the RP2040 have?
- What is a DMA "control block" / "chain"? Why is chaining useful for audio?
- What is the difference between a DREQ-triggered DMA transfer and a software-triggered one?
- How does the DMA interact with the I2S peripheral (hint: PIO)?

**Section §3.5 (PIO):**
- What is PIO and why does the RP2040 use it instead of a dedicated I2S peripheral?
- What is a PIO state machine?

> **There is no length requirement.** Depth of understanding matters more than word count. Three sentences that demonstrate real understanding beat a paragraph of rephrased datasheet text.

---

### Part 3 — AI Reflection Journal (Week 2)

In `journal/week2.md`, add your weekly reflection (1–2 paragraphs):
- What did you ask Copilot this week (lab + homework)?
- One case where Copilot was confidently wrong — what was it, and how did you catch it?
- One case where Copilot saved significant time.

---

### Commit Checklist

By Monday April 13 at 11:00 a.m.:

- [ ] `2026_board_README.md` — final polish with status column and Firmware Notes
- [ ] `assignment2/rp2040_notes.md` — DMA and PIO answers
- [ ] `journal/week2.md` — AI reflection

---

### Looking Ahead
In Lab 3 (April 14), you will write a DMA memory transfer in C and use the Pico Debug Stack's SWD probe to step through it in VS Code — watching DMA channel registers change in real-time. Come with your RP2040 DMA notes fresh.
