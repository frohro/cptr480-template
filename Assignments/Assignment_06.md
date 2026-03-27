# CPTR 480 — Assignment 6
## Audio Pipeline Architecture + Timing Analysis
**Assigned:** Monday, May 4 | **Due:** Monday, May 11 at 11:00 a.m.

---

### Overview
Lab 6 gets the UAC2+CDC composite device enumerating. Lab 7 adds live audio. This assignment asks you to design and analyze the full audio data path *before* you implement it, so you arrive at Lab 7 with a plan, not a guess.

> *Specific questions and deliverable details will be finalized and posted to the course repo by Wednesday May 6.*

---

### Part 1 — End-to-End Data Path Design

Draw and document the complete audio pipeline in `assignment6/pipeline_design.md`. Your diagram must show:

1. PCM1808 → PIO state machine → DMA channel A/B (ping-pong)
2. DMA interrupt handler on Core 0 (or Core 1) → inter-core FIFO or shared ring buffer
3. TinyUSB isochronous transfer callback → USB packet assembly → host

For each stage, annotate:
- Buffer size (in bytes and in samples)
- How often data moves (rate in Hz or bytes/ms)
- Which core runs the code at that stage
- What happens if that stage is late

> **Deliverable:** `assignment6/pipeline_design.md` (ASCII diagram acceptable; image preferred) committed.

---

### Part 2 — Timing Budget

In `assignment6/timing_analysis.md`:

- The TinyUSB isochronous callback fires every 1 ms. How many 32-bit stereo samples must be ready in that window at 48 kHz?
- Estimate (don't measure yet) the worst-case time for your DMA interrupt handler. What is the margin before it starts missing USB frames?
- What is the inter-core FIFO depth on the RP2040? If Core 1 fills the FIFO and Core 0 isn't draining it, what happens?
- Identify the single most likely point of failure in your pipeline design. What observable symptom would it produce (audio dropout, click, silence, etc.)?

> **Deliverable:** `assignment6/timing_analysis.md` committed.

---

### Part 3 — AI Reflection Journal (Week 6)

In `journal/week6.md`:
- Did the AI help you design the pipeline, or did it produce a plausible but incorrect architecture? What did you have to correct?
- Reflecting on Labs 6 and 7 so far: what has been harder than expected, and what was easier?

---

### Commit Checklist

By Monday May 11 at 11:00 a.m.:

- [ ] `assignment6/pipeline_design.md`
- [ ] `assignment6/timing_analysis.md`
- [ ] `journal/week6.md`
