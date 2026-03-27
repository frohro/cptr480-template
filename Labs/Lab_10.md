# CPTR 480 — Lab 10
## Demo Video + Final Presentation Slides
**Date:** Tuesday, June 2, 2026 | 2:00–5:00 p.m.
**Due:** All deliverables committed by **8:00 a.m., Wednesday, June 10** (start of final presentations)

> Final presentations are **Wednesday, June 10 at 8:00 a.m.** All code, the README, slides, and the demo video link must be committed before you arrive. Do not travel before your presentation slot.

---

### Learning Objectives
By the end of this lab you will be able to:
- Record and narrate a concise technical demo video that stands alone without you present
- Construct a slide deck that communicates system architecture and results to a technical audience
- Deliver a live 8-minute final presentation with a hardware demo

---

### What You Need
- A working (or partially working) extension project
- Screen recording software (`ffmpeg`, OBS Studio, or any preference)
- A camera or phone for hardware footage if relevant
- Slide editor of your choice (LibreOffice Impress, Google Slides, PowerPoint, etc.)

---

### Overview

Lab 10 is a production session. The instructor and helpers are available for last-minute debugging, but the bulk of the lab time is yours to:
1. Finish any remaining extension code.
2. Record your demo video.
3. Draft your slides.
4. Do at least one dry run of your live presentation.

The course ends with final presentations. Every team presents for **8 minutes** plus **4 minutes** of Q&A. The audience is your classmates, the instructor, and any invited guests.

---

## Part 1 — Code Freeze and Final Commit (~30 min)

Before recording anything, get your code into a final state:

1. Ensure all lab deliverables (Labs 1–10) are committed and pushed.
2. Tag the final state: `git tag final-submission && git push origin final-submission`
3. Write `lab10/final_state.md` answering:
   - What is fully working?
   - What is partially working (and to what degree)?
   - What did not get done, and why?
   - What would you do differently next time?

This is not a self-grade — it is context for the instructor and is graded for honesty and specificity, not outcomes.

---

## Part 2 — Demo Video (~60 min)

Record a **4–6 minute demo video** that could be watched by someone who was not in this class. It must stand alone — assume the viewer has not read your proposal.

**Required content:**
1. **System overview (≤ 60 sec):** What does the hardware do? Show the board.
2. **Phase 1 demo (≤ 90 sec):** Quisk waterfall live with your UAC2 firmware. Tune to an actual signal.
3. **Extension demo (≤ 2 min):** Show your extension working. If it is partially working, show the part that works and briefly explain what remains.
4. **Architecture callout (≤ 60 sec):** Point to one interesting piece of code or circuit and explain why you did it that way.

**Recording tips:**
- Use OBS Studio for screen + webcam composite, or record screen and hardware separately and edit.
- Narrate live — reading a script sounds flat; bullet points work better.
- Keep the SD card reader icon muted and close Slack notifications before recording.
- `ffmpeg` can trim and concatenate clips without quality loss:
  ```bash
  ffmpeg -i part1.mp4 -i part2.mp4 -filter_complex concat=n=2:v=1:a=1 demo.mp4
  ```

**Upload / link:**
- Commit the video file directly if it is ≤ 100 MB (use `.gitignore` exceptions if needed).
- Or upload to YouTube (unlisted) or Google Drive (anyone with link can view) and commit the URL to `lab10/demo_video_url.txt`.

> **Deliverable:** `lab10/demo_video_url.txt` or the video file in `lab10/`.

---

## Part 3 — Presentation Slides (~45 min)

Your final presentation slides are **8–10 slides** for an 8-minute talk. Less is more.

**Suggested slide structure:**

| Slide | Content |
|---|---|
| 1 | Title: project name, team members, date |
| 2 | System block diagram — one picture showing hardware + firmware + software stack |
| 3 | Phase 1 result — photo or screenshot of Quisk waterfall; one architecture callout |
| 4 | Extension: what problem does it solve / what does it add? |
| 5 | Technical approach — key design decision with alternatives you rejected |
| 6 | Firmware architecture — relevant subset of the data flow diagram |
| 7 | Results — what works, quantified (e.g., "48 kHz stereo, <1 ms latency, no dropouts") |
| 8 | What didn't work / what you'd do differently |
| 9 | Demo slide (live demo happens here) |
| 10 | Questions |

**Slide design rules:**
- No paragraph text on slides. Bullets of ≤8 words.
- Every diagram must have axis labels or a legend.
- Font ≥ 24 pt. Contrast ratio ≥ 4.5:1 (dark text on light background or vice versa).
- No more than 3 colors in a diagram.

Commit slides as PDF: `lab10/slides.pdf`. Also commit the source file (`.pptx`, `.odp`, `.key`, or link) so you can edit it before the presentation.

> **Deliverable:** `lab10/slides.pdf`

---

## Part 4 — Live Presentation Dry Run (~30 min)

Before the end of lab, do at least one complete dry run with your partner:
- One person presents, one person watches and times with a phone.
- The goal is ≤ 8 minutes for the prepared portion and a live hardware demo that starts within 30 seconds.
- Note anything that felt rushed or confusing and fix it.

**Common issues:**
- Demo setup takes 3 minutes (USB enumeration, Quisk startup). Practice the setup so it is under 30 seconds.
- Slide 2 (block diagram) takes 4 minutes because you explain every box. Trim — cover only the interesting parts.
- The extension demo fails live because a USB device was not plugged in. Always have a backup recording.

Write one sentence in `lab10/lab10_report.md` about what you fixed after the dry run.

---

## Part 5 — Final Presentation Day

**Logistics:**
- Bring your board, all cables, and the host laptop.
- Plug in and enumerate before your slot starts — do not do hardware setup while the clock is running.
- Have your slides open full-screen before you stand up.
- Have the demo video queued as a backup in case the hardware fails.

**Presentation grading rubric:**

| Criterion | Weight | What earns full marks |
|---|---|---|
| Technical correctness | 30% | Claims about hardware/firmware are accurate; architecture is clearly explained |
| Demonstrated results | 25% | Live demo works (or backup video shows it working) |
| Extension depth | 20% | Extension goes meaningfully beyond Phase 1; design decisions justified |
| Communication clarity | 15% | Slides are legible; narration is clear; time within 8–10 min |
| Q&A responses | 10% | Can answer follow-up questions about implementation details |

---

### Commit Checklist

Before noon on your final presentation day:

- [ ] `final-submission` git tag pushed to GitHub
- [ ] `lab10/final_state.md` — honest assessment of what works and what doesn't
- [ ] `lab10/demo_video_url.txt` (or video file in `lab10/`) — 4–6 min demo video
- [ ] `lab10/slides.pdf` — final presentation slides
- [ ] `lab10/slides.*` — source file (pptx / odp / key / link)
- [ ] `lab10/lab10_report.md` — dry run notes
- [ ] All Labs 1–9 deliverables present and committed (do a final check: `git log --oneline`)
