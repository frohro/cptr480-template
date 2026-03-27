# CPTR 480 — Assignment 4
## PCM1808 + PIO I2S Deep Dive + USB Fundamentals
**Assigned:** Monday, April 20 | **Due:** Monday, April 27 at 11:00 a.m.

---

### Overview
This assignment prepares you for the USB CDC lab (Lab 5) by solidifying your understanding of PIO-based I2S and getting ahead on USB fundamentals. Three parts: PCM1808 datasheet study, PIO I2S driver analysis, and USB endpoint/descriptor concepts.

> *Specific questions and deliverable details will be finalized and posted to the course repo by Wednesday April 22.*

---

### Part 1 — PCM1808 Datasheet Study

Using the [PCM1808 datasheet](https://www.ti.com/product/PCM1808), answer in `assignment4/pcm1808_notes.md`:

- What is the PCM1808's default audio format (I2S, left-justified, etc.) after power-on with all mode pins floating/default?
- What sample rates does it support, and what master clock frequencies correspond to each at default oversampling?
- What is the output word length? How does this map to what arrives on the I2S DATA line per frame?
- What voltage levels are required on FMT0/FMT1/MD0/MD1 for standard I2S 256fs on the 2026 board? Check the schematic.

> **Deliverable:** `assignment4/pcm1808_notes.md` committed.

---

### Part 2 — PIO I2S Driver Analysis

Review the PIO I2S driver from Lab 4 (or the pico-extras reference implementation). Answer in `assignment4/pio_notes.md`:

- Trace through the PIO state machine program instruction by instruction. What does each instruction do in terms of SCK, LRCK, and DATA?
- How does the PIO program know when a left vs. right sample starts?
- Where in the DMA configuration is the 32-bit word size enforced? What would happen if you changed it to 16-bit?
- The PCM1808 outputs 24 bits of valid data. Where do the top 8 bits of the 32-bit PIO word end up? Are they zero, sign-extended, or garbage?

> **Deliverable:** `assignment4/pio_notes.md` committed.

---

### Part 3 — USB Fundamentals Reading

Read RP2040 Datasheet §4 (USB) introduction and the TinyUSB documentation overview. Answer in `assignment4/usb_notes.md`:

- What is the difference between a bulk endpoint and an isochronous endpoint? Which does CDC use? Which does UAC2 use, and why?
- What is a USB descriptor? List the descriptor types a UAC2 composite device must present during enumeration.
- What does "enumeration" mean? Walk through the sequence of requests a host makes when a USB device is plugged in.

> **Deliverable:** `assignment4/usb_notes.md` committed.

---

### Part 4 — AI Reflection Journal (Week 4)

In `journal/week4.md`:
- What did the AI help you understand this week that you couldn't have parsed from the datasheet alone?
- Where did the AI give you a plausible-sounding but incorrect answer about USB or PIO? How did you verify it?

---

### Commit Checklist

By Monday April 27 at 11:00 a.m.:

- [ ] `assignment4/pcm1808_notes.md`
- [ ] `assignment4/pio_notes.md`
- [ ] `assignment4/usb_notes.md`
- [ ] `journal/week4.md`
