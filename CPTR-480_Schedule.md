# CPTR 480 — Programming Embedded and Real-Time Systems
## Spring Quarter 2026 Schedule

**Room:** CSP163 | **Lecture:** MWF 11:00–11:50 a.m. | **Lab:** CSP316 | Tue 2:00–5:00 p.m.

**Project Goal:** **Phase 1 (Weeks 1–7):** A Raspberry Pi Pico clone (YD-RP2040) on the WWU 2026 SDR board that enumerates as a USB Audio Class 2 composite device (I/Q audio stream + CDC control channel), tunable from Quisk on a host PC. **Phase 2 (Weeks 8–10):** Each student extends the project in a self-chosen direction; the final presentation showcases both phases. Extension ideas include: 24-bit/96 kHz audio, WiFi UDP audio via Pico W/Pico 2W, standalone SDR (OLED + no PC), full sound card (DAC playback), music player, or other creative uses of the 2026 board.

**Grading:** 50% Labs (GitHub commits) | 20% HW assignments (GitHub commits) | 15% AI reflection journal | 15% final demo + presentation  
*The final presentation rubric explicitly rewards technical depth and extension ambition. The base Quisk-capable SDR represents solid mid-range performance; extensions are expected and scored accordingly.*

**Teams:** Students work in self-selected teams of two (declared by Lab 3, Monday April 13). Each team shares a GitHub repository; both partners present at the final.

**Workload:** This is a 4-credit course. Expect **12 hours per week** — 3 hrs lecture + 3 hrs lab + ~6 hrs outside work. The project builds incrementally every week. **Students who defer effort cannot recover with a sprint in week 10** — UAC2 alone has defeated students who tried.

**Due Dates:** Regular assignments are due the **next Monday at 11:00 a.m.** Labs are due **one week after the lab session (the following Tuesday) at 2:00 p.m.**

---

| Wk | Date | Day | Topic | Due |
|----|------|-----|-------|-----|
| 1 | Mon Mar 30 | Lec | Course intro; embedded systems overview; AI/Copilot workflow; toolchain + GitHub setup | — |
| 1 | Tue Mar 31 | **Lab 1** | **2026 Board First Light** (header soldering, MicroPython, power rails, OLED, RGB LED, encoder) | — |
| 1 | Wed Apr 1 | Lec | Board bring-up methodology; 2026 board schematic walkthrough | — |
| 1 | Fri Apr 3 | Lec | Logic analyzer; I2C/SPI fundamentals; 2026 board module map | — |
| 2 | Mon Apr 6 | Lec | Embedded documentation; board README; Copilot-assisted documentation workflow | **HW 1 due 11:00 a.m.** |
| 2 | Tue Apr 7 | **Lab 2** | **Pico Debug Stack Setup + Full Board Validation + README** | **Lab 1 due 2:00 p.m.** |
| 2 | Wed Apr 8 | Lec | Real-time constraints; C/C++ Pico SDK intro; CMakeLists.txt project structure | — |
| 2 | Fri Apr 10 | Lec | RP2040 architecture: memory map, clock tree, bus fabric; Pico SDK HAL overview; I2C peripheral in C | — |
| 3 | Mon Apr 13 | Lec | Si5351a PLL architecture; SDR local oscillator theory; Copilot-assisted driver development | **HW 2 due 11:00 a.m.** |
| 3 | Tue Apr 14 | **Lab 3** | **Si5351a I2C Driver** | **Lab 2 due 2:00 p.m.** |
| 3 | Wed Apr 15 | Lec | SWD debugging: breakpoints, watchpoints, memory inspection, hard faults; verifying I2C register writes live | — |
| 3 | Fri Apr 17 | Lec | DMA fundamentals: channels, control blocks, ring buffers, chaining, interrupts | — |
| 4 | Mon Apr 20 | Lec | I2S protocol; PCM1808 configuration; PIO-based I2S driver; DMA ping-pong for continuous audio | **HW 3 due 11:00 a.m.** |
| 4 | Tue Apr 21 | **Lab 4** | **PCM1808 Audio Capture** | **Lab 3 due 2:00 p.m.** |
| 4 | Wed Apr 22 | Lec | Real-time deadline analysis; profiling the audio pipeline; RP2040 dual-core + Core 1 audio offload | — |
| 4 | Fri Apr 24 | Lec | USB fundamentals: endpoints, descriptors, enumeration, transfer types | — |
| 5 | Mon Apr 27 | Lec | TinyUSB library structure; CDC bulk endpoints; command dispatch; wiring `tune` to Si5351a | **HW 4 due 11:00 a.m.** |
| 5 | Tue Apr 28 | **Lab 5** | **USB CDC + Command Parsing** — tune Si5351a via CDC; live Quisk demo with borrowed sound card | **Lab 4 due 2:00 p.m.** |
| 5 | Wed Apr 29 | Lec | USB Audio Class 2 (UAC2): isochronous endpoints, feedback endpoint, clock entities, descriptors | — |
| 5 | Fri May 1 | Lec | Composite USB devices (CDC + UAC2); clock domain crossing; feedback endpoint math | — |
| 6 | Mon May 4 | Lec | UAC2 enumeration troubleshooting; `lsusb -v` interpretation; common descriptor mistakes | **HW 5 due 11:00 a.m.** |
| 6 | Tue May 5 | **Lab 6** | **UAC2+CDC Part 1 — Enumeration** — composite device enumerates on host; descriptors verified with `lsusb -v`; `tune` command accepted over CDC | **Lab 5 due 2:00 p.m.** |
| 6 | Wed May 6 | Lec | Audio pipeline architecture: connecting PCM1808 DMA output to UAC2 isochronous stream | — |
| 6 | Fri May 8 | Lec | Inter-core FIFO; buffer management between DMA callback and USB isochronous transfer | — |
| 7 | Mon May 11 | Lec | Full integration walkthrough: ADC → DMA → UAC2 → Quisk; Quisk protocol and CDC command interface | **HW 6 due 11:00 a.m.** |
| 7 | Tue May 12 | **Lab 7 ★** | **UAC2+CDC Part 2 — Audio + Quisk (SDR Milestone ★)** — PCM1808 audio streams through UAC2; Quisk waterfall live on host; tunable via CDC | **Lab 6 due 2:00 p.m.** |
| 7 | Wed May 13 | Lec | QSD SDR I/Q theory; image rejection; why matched channels matter | — |
| 7 | Fri May 15 | Lec | Extension project brainstorm: options, difficulty tiers, hardware choices (Pico W / Pico 2 / Pico 2W available for checkout); bring a project idea | — |
| 8 | Mon May 18 | Lec | Extension project kickoff: survey of chosen directions; hardware checkout; 24-bit/UAC2 deep dive; lwIP/WiFi overview for Pico W projects | **HW 7 due 11:00 a.m.** |
| 8 | Tue May 19 | **Lab 8** | **Extension Project Launch** — chosen direction committed to GitHub (one-paragraph proposal + plan + hardware selection) | **Lab 7 due 2:00 p.m.** |
| 8 | Wed May 20 | Lec | Open project work + instructor consultations | — |
| 8 | Fri May 22 | Lec | Project work | — |
| 9 | Mon May 25 | *No class* | Memorial Day | — |
| 9 | Tue May 26 | **Lab 9** | **Peer Design Review + Extension Midpoint** — first 30 min: read partner team's review doc (due 2:00 p.m. today); structured discussion + response document committed to GitHub; remainder: project work | **Lab 8 + HW 8 (Design Review doc) due 2:00 p.m.** |
| 9 | Wed May 27 | Lec | Project work| — |
| 9 | Fri May 29 | Lec | Project work | — |
| 10 | Mon Jun 1 | Lec | Project work | — |
| 10 | Tue Jun 2 | **Lab 10** | **Demo Video + Presentation Slides** | **Lab 9 due 2:00 p.m.** |
| 10 | Wed Jun 3 | Lec | Project work | — |
| 10 | Fri Jun 5 | Lec | Project work | — |
| Finals | Wed June 10, 8:00 a.m | **Presentations** | Live demo + presentation to class and invited guests | **Lab 10 + all code + README + slides due** |

---

*Schedule subject to adjustment. **HW assignments** (HW 1–8) are weekly, assigned on Monday, and due the next Monday at **11:00 a.m.**. **Labs** are on Tuesdays, and due the following Tuesday at **2:00 p.m.** All deliverables are to be committed to your GitHub repo by the stated due time.*  
*Phase 2 extension projects are expected of all students. Students still debugging the base SDR in weeks 8–10 should focus on getting it solid — a polished, working base SDR is their extension. Hardware (Pico W, Pico 2, Pico 2W) is available for checkout from the instructor for approved extension projects.*
