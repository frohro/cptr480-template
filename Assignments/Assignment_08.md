# CPTR 480 — Assignment 8
## Peer Design Review
**Assigned:** Tuesday, May 19 (after Lab 8 proposals are committed) | **Due:** Tuesday, May 26 at 2:00 p.m. (start of Lab 9)

---

### Overview
You will be assigned a partner team to review. Your job is to read their extension proposal (committed at Lab 8) carefully, study their existing code in their GitHub repository, find real problems, and write a substantive review document. This is not a courtesy read — it is a technical inspection.

**You are expected to spend at least one hour on this.** A review that could have been written in ten minutes will be graded accordingly.

The team you review will read your review, respond to it at Lab 9, and recommend a grade for your review quality. The instructor has final grading authority but takes the recommendation seriously.

---

### Who Reviews Whom

Review pairs will be announced in class on Monday May 18 or posted to the course channel by end of day. Each team reviews one team; teams are arranged in a chain so no team reviews back their own reviewer.

**Repository access — action required by the reviewed team:**  
Once pairs are announced, the team being reviewed must add both members of the reviewing team as GitHub collaborators with **Read** access by end of day Monday May 18. Go to your repo → Settings → Collaborators and teams → Add people → select Read. This gives reviewers access to your code, not just your proposal document. Reviewers may not fork or clone your repo for any purpose other than reading.

---

### What to Review

You are reviewing your partner team's `assignment7/extension_proposal.md` and any relevant code already in their repository (Si5351a driver, UAC2 descriptors, DMA code, etc.).

---

### Review Document

Commit your review to **your own repository** at `assignment8/design_review.md`. It must contain the following sections:

---

#### Section 1 — Summary of Their Approach
*In your own words* (2–4 sentences), describe what the partner team is building and how they plan to do it. This proves you read and understood the proposal. Do not copy their text.

---

#### Section 2 — Technical Risks and Potential Bugs
Identify **at least two** specific technical risks or potential problems. Each must be:
- **Specific** — not "this might be hard" but "the 10.14 fixed-point feedback value assumes 48 kHz; at 96 kHz the nominal value changes and if they don't recalculate it, the host will see clock drift."
- **Reasoned** — explain *why* it is a risk, referencing the datasheet, spec, or their code
- **Actionable** — suggest what they should check or do to mitigate it

---

#### Section 3 — Suggestions and Ideas
Offer **at least one** concrete suggestion or alternative approach they may not have considered. This can be a simpler path to their goal, a useful library or reference implementation, a hardware consideration, or a testing strategy.

---

#### Section 4 — Questions for Lab 9
List **at least three** questions you will ask them in the Lab 9 review session. These should be questions you genuinely want answered — questions that would change how you evaluate their plan.

---

#### Section 5 — Grade Recommendation
Recommend a letter grade for the partner team's **proposal quality** (Assignment 7), with a 2–3 sentence justification. Consider: Was the proposal specific enough to review? Were risks identified? Was the approach technically sound?

Your recommendation is for their *proposal*, not your prediction of their final project outcome.

---

### Grading of Your Review

Your review (Assignment 8) will be graded by the instructor on:

| Criterion | Weight |
|-----------|--------|
| Technical depth — did you find real issues, not surface observations? | 40% |
| Specificity — are risks and suggestions concrete and reasoned? | 30% |
| Fairness and professionalism | 15% |
| Completeness — all required sections present | 15% |

The reviewed team's grade recommendation for *your review quality* is also considered (approx. 50/50 blend with instructor assessment).

---

### Lab 9 Session (Tuesday May 26 — First Hour)

Each review pair meets for approximately 20 minutes:
- Reviewing team presents findings verbally (10 min): risks, suggestions, questions
- Reviewed team responds (10 min): which issues they agree with, which they dispute, and what they plan to do about each

After the session, the reviewed team commits a response document to their own repo at `assignment8/review_response.md` (due end of Lab 9). The response must address each issue raised and include their grade recommendation for the reviewing team's work.

---

### Commit Checklist

By end of day Monday May 18:

- [ ] Reviewed team has added both reviewers as GitHub collaborators (Read access)

By Tuesday May 26 at 2:00 p.m. (committed before you arrive at lab):

- [ ] `assignment8/design_review.md` committed to **your** repository — all five sections complete

By end of Lab 9 (Tuesday May 26):

- [ ] `assignment8/review_response.md` committed to **your** repository — response to the review your team received
