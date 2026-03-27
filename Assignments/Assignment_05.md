# CPTR 480 — Assignment 5
## UAC2 Descriptor Study + Quisk Protocol
**Assigned:** Monday, April 27 | **Due:** Monday, May 4 at 11:00 a.m.

---

### Overview
Labs 6 and 7 are a two-week UAC2 challenge — the hardest part of the course. This assignment is your preparation. You are expected to understand UAC2 descriptor structure before Lab 6, not during it.

> *Specific questions and deliverable details will be finalized and posted to the course repo by Wednesday April 29.*

---

### Part 1 — UAC2 Descriptor Deep Dive

Read the [USB Audio Class 2.0 specification](https://www.usb.org/document-library/audio-devices-rev-30-and-adopters-agreement) summary sections and the TinyUSB UAC2 example. Answer in `assignment5/uac2_notes.md`:

- What descriptors must a UAC2 device present that a simple CDC device does not? List them in enumeration order.
- What do `bSubframeSize` and `bBitResolution` mean, and what values would you use for 32-bit container / 24-bit valid audio?
- What is the UAC2 feedback endpoint? Why is it needed for isochronous audio on USB Full Speed?
- What is `wMaxPacketSize` for a 48 kHz stereo 32-bit isochronous endpoint on Full Speed USB? Show the calculation.
- What is the difference between a Clock Source entity and a Clock Selector entity in a UAC2 topology?

> **Deliverable:** `assignment5/uac2_notes.md` committed.

---

### Part 2 — Composite Device Structure

In `assignment5/composite_notes.md`:

- How does a composite USB device present both CDC and UAC2 in one set of descriptors? What descriptor ties them together?
- What is Interface Association Descriptor (IAD) and why is it required for composite devices on modern hosts?
- What pitfalls does TinyUSB's documentation warn about for composite CDC + Audio configurations?

> **Deliverable:** `assignment5/composite_notes.md` committed.

---

### Part 3 — Quisk Data Requirements

In `assignment5/quisk_notes.md`, research Quisk's source or documentation:

- What sample rate does Quisk request from the SDR sound card by default? Is this configurable?
- What CDC commands does Quisk send to control the SDR? (At minimum: the `tune` command format.)
- At 48 kHz stereo 32-bit, how many bytes per second must flow over the USB isochronous endpoint to keep Quisk happy? Cross-check this with the USB bandwidth budget.

> **Deliverable:** `assignment5/quisk_notes.md` committed.

---

### Part 4 — AI Reflection Journal (Week 5)

In `journal/week5.md`:
- UAC2 is famously complex. Where did the AI help you parse the spec, and where did it confidently give you wrong field values?
- What's one thing about UAC2 you now understand that you didn't before this assignment?

---

### Commit Checklist

By Monday May 4 at 11:00 a.m.:

- [ ] `assignment5/uac2_notes.md`
- [ ] `assignment5/composite_notes.md`
- [ ] `assignment5/quisk_notes.md`
- [ ] `journal/week5.md`
