# CPTR 480 — Lab 9
## Peer Design Review + Extension Project Midpoint
**Date:** Tuesday, May 26, 2026 | 2:00–5:00 p.m.
**Due:** `review_response.md` committed by **end of lab today (≈ 5:00 p.m.)**

---

### Learning Objectives
By the end of this lab you will be able to:
- Evaluate a peer team's project proposal using specific technical criteria
- Give and receive constructive feedback in a structured format
- Articulate the current state of your extension project and what remains to be done

---

### What You Need
- Assignment 8 (your written peer review) — **must be committed before you arrive**
- Read access to your assigned review target's GitHub repo (granted by end-of-day May 18)
- A working (or partially working) extension project to demo or describe

> **Assignment 8 is a prerequisite for participating in today's discussion.** If it is not committed you will not be able to complete the review session effectively. The first 30 minutes of lab are reserved for reading, not writing — Assignment 8 should already exist.

---

### Schedule

| Time | Activity |
|---|---|
| 2:00 – 2:30 | Silent reading period: read your assigned team's proposal and your written review. Review pairs also read each other's Assignment 8. |
| 2:30 – 4:30 | Structured peer review sessions (see Part 2) |
| 4:30 – 5:00 | Extension project midpoint demos + `review_response.md` committed |

---

## Part 1 — Reading Period (2:00–2:30, silent)

Use this time to:
1. Re-read the proposal you reviewed in Assignment 8.
2. Re-read your own Assignment 8 to remind yourself what questions you asked.
3. Look at the reviewed team's GitHub repo — has any code been committed since the proposal? Does it match what they proposed?
4. Note any new questions or observations on paper.

Do not discuss with other teams during this period.

---

## Part 2 — Structured Review Sessions (2:30–4:30)

Each review pair has **20 minutes** total. Instructor will keep time and rotate.

**Format:**
- **Minutes 0–10 (Reviewing team presents):**
  The reviewing team summarizes the proposal in their own words, states their ≥2 technical risks from Assignment 8, gives their ≥1 improvement suggestion, and asks their ≥3 questions. The reviewed team listens and takes notes — do not interrupt.

- **Minutes 10–20 (Reviewed team responds):**
  The reviewed team answers the questions, responds to the risks (agree / disagree / mitigated), and explains any changes made since the proposal. The reviewing team may ask brief follow-up questions.

**Ground rules:**
- Be specific. "Your Pico W UDP code might not work" is not a useful critique. "Your UDP code sends 384 bytes every 1 ms, but the CYW43 WiFi driver has a minimum ~4 ms latency per packet — this will cause 4× buffer overrun" is useful.
- Be fair. If a plan is technically sound, say so.
- The goal is to make each other's projects better, not to win.

**Review chain (assigned):** A→B→C→...→A. Check with instructor for your specific assignment if you have lost track.

---

## Part 3 — Write `review_response.md` (during or after discussion)

After your review session, the **reviewed team** (i.e., the team whose proposal was reviewed) commits `review_response.md` to their repo. Due by end of lab (5:00 p.m.).

```markdown
# Peer Review Response

## Reviewer team
(Names of the team that reviewed your proposal)

## Risk responses
For each risk identified in their Assignment 8: do you agree it is a real risk?
Have you mitigated it? How?

## Suggestion response
What do you think of their ≥1 improvement suggestion? Will you incorporate it?

## Question answers
Answer each of their ≥3 Lab 9 questions.

## Plan updates
As a result of this review, what (if anything) changes in your extension plan?
```

> **Both** teams should commit a note: the reviewing team commits a brief `lab9_review_notes.md` with any follow-up observations from the discussion; the reviewed team commits `review_response.md`.

---

## Part 4 — Extension Midpoint Demo (4:30–5:00)

In the last 30 minutes, each team gives a quick (≤ 2 min) informal status update to the class:

1. What does your extension do (one sentence)?
2. What is working right now? (Live demo if possible — even if partial)
3. What is the one thing that must work by Week 10?
4. What help do you need?

There are no grades attached to this demo — it is a progress check and a chance to get peer help. But it informs the instructor's grading of your final presentation effort.

---

### Commit Checklist

By 5:00 p.m. today:

**Reviewed team commits to their own repo:**
- [ ] `review_response.md` — responses to risks, suggestions, and questions raised by reviewers

**Reviewing team commits to their own repo:**
- [ ] `lab9_review_notes.md` — any additional observations or follow-ups from the live discussion

**Both teams:**
- [ ] Extension branch has new code or documentation commits since Lab 8 (shows ongoing progress)
