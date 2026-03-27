# CPTR 480 — Assignment 1
## Dev Environment Setup + Datasheet Study + 2026 Board README Skeleton
**Assigned:** Monday, March 30 | **Due:** Monday, April 6 at 11:00 a.m.

---

### Overview
This assignment runs alongside Lab 1. Most of it is done outside of lab time. It has four parts: (1) installing your complete development environment, (2) finishing Copilot Pro setup, (3) reading and summarizing key datasheets using Copilot as a research tool, and (4) starting a README for the 2026 SDR board that will be completed during Lab 2.

**Complete Part 1 before Lab 1 (Tuesday March 31) if at all possible.** Arriving at lab with a working toolchain — and KiCad open on your laptop — means you spend lab time on hardware, not installation.

---

### Part 1 — Development Environment Setup

Complete all sections for your operating system. Each section ends with a verification test — do not skip them.

---

#### 1a — VS Code

Download and install **Visual Studio Code** from [code.visualstudio.com](https://code.visualstudio.com). Accept the default options.

**Install these extensions** (open VS Code → Extensions sidebar → search each name):

| Extension | Publisher | Purpose |
|-----------|-----------|---------|
| `GitHub Copilot` | GitHub | AI code completion |
| `GitHub Copilot Chat` | GitHub | AI chat assistant |
| `C/C++` | Microsoft | C/C++ IntelliSense and debugging |
| `CMake Tools` | Microsoft | CMake build integration |
| `Cortex-Debug` | marus25 | ARM SWD debugging (debug probe) |
| `MicroPico` | paulober | MicroPython on the Pico |
| `Python` | Microsoft | Python scripting support |
| `KiCad Schematic Viewer` | Simon Hocke | View `.kicad_sch` files without leaving VS Code (optional but handy) |

> **Verify:** Open the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`), type `Copilot: Open Chat`. A chat panel should open and respond to a message.

---

#### 1b — GitHub Account + Copilot

1. Create a GitHub account at [github.com](https://github.com) if you don't have one.  
2. Go to [education.github.com/students](https://education.github.com/students) → **Get student benefits** → upload your current class schedule or transcript. Approval is usually 1–3 business days.
3. While waiting for Pro approval, activate the **free tier** of Copilot: [github.com/settings/copilot](https://github.com/settings/copilot).
4. In VS Code, sign in to GitHub (Accounts icon bottom-left → Sign in with GitHub).

> **Verify:** Ask Copilot Chat: *"What is DMA?"* — it should respond.

---

#### 1c — Pico SDK Toolchain

The Pico SDK requires: `git`, `cmake`, `arm-none-eabi-gcc`, and `ninja` (or `make`). Follow the instructions for your OS:

---

##### Windows

The easiest and most reliable method is the **official Raspberry Pi Pico Windows installer**:

1. Download the **Pico Setup for Windows** installer from:  
   [github.com/raspberrypi/pico-setup-windows/releases](https://github.com/raspberrypi/pico-setup-windows/releases)  
   Download the `.exe` for the latest release.

2. Run the installer. It installs: the Pico SDK, `arm-none-eabi-gcc`, CMake, Ninja, Git, and VS Code CMake configuration — all at once.

3. After installation, open the **"Pico - Visual Studio Code"** shortcut it creates (not the regular VS Code). This opens VS Code with `PICO_SDK_PATH` pre-set.

4. From the Pico VS Code shortcut, open a terminal and run:
   ```cmd
   cmake --version
   arm-none-eabi-gcc --version
   ```
   Both should print version numbers.

> **If you use WSL2:** The toolchain works in WSL2 but USB port access for flashing requires extra setup. Stick with native Windows for this class.

---

##### macOS

1. Install **Homebrew** if not already installed: [brew.sh](https://brew.sh)

2. Install the toolchain:
   ```bash
   brew install cmake ninja git
   brew install --cask gcc-arm-embedded
   ```

3. Clone the Pico SDK to your home directory:
   ```bash
   cd ~
   git clone --recursive https://github.com/raspberrypi/pico-sdk.git
   ```

4. Add `PICO_SDK_PATH` to your shell profile. For zsh (default on macOS):
   ```bash
   echo 'export PICO_SDK_PATH="$HOME/pico-sdk"' >> ~/.zshrc
   source ~/.zshrc
   ```

5. Verify:
   ```bash
   cmake --version          # should be ≥ 3.13
   arm-none-eabi-gcc --version
   echo $PICO_SDK_PATH      # should print the path
   ```

6. Install **OpenOCD** (needed for the debug probe in Lab 3+):
   ```bash
   brew install openocd
   ```

---

##### Linux (Ubuntu / Debian / Mint)

1. Install dependencies:
   ```bash
   sudo apt update
   sudo apt install -y git cmake ninja-build build-essential \
       gcc-arm-none-eabi libnewlib-arm-none-eabi \
       libstdc++-arm-none-eabi-newlib openocd python3
   ```

2. Clone the Pico SDK:
   ```bash
   cd ~
   git clone --recursive https://github.com/raspberrypi/pico-sdk.git
   ```

3. Add `PICO_SDK_PATH` to your shell profile:
   ```bash
   echo 'export PICO_SDK_PATH="$HOME/pico-sdk"' >> ~/.bashrc
   source ~/.bashrc
   ```

4. Verify:
   ```bash
   cmake --version          # should be ≥ 3.13
   arm-none-eabi-gcc --version
   echo $PICO_SDK_PATH
   ```

> **Note:** Ubuntu 22.04's `arm-none-eabi-gcc` package is version 11 — sufficient for this class. Ubuntu 20.04 users may need to install from the Arm developer site instead.

---

#### 1d — MicroPython + MicroPico (needed for Labs 1–2)

MicroPython does **not** use the Pico SDK toolchain — it's a separate firmware you flash once.

1. Download the latest **MicroPython UF2** for the RP2040 Pico from:  
   [micropython.org/download/RPI_PICO](https://micropython.org/download/RPI_PICO/)  
   (Download the `.uf2` file — it's a few hundred KB.)

2. The **MicroPico** VS Code extension (installed in 1a) handles everything else — it auto-detects the board, uploads scripts, and opens a REPL. No additional install needed.

> **Verify (do at Lab 1):** Hold BOOTSEL on the Pico while plugging in USB → `RPI-RP2` drive appears → drag `.uf2` onto it → Pico reboots → MicroPico in VS Code connects automatically.

---

#### 1f — KiCad (needed for Lab 1 and throughout)

You will read and trace the 2026 board schematic during Lab 1 to find GPIO pin assignments. KiCad must be installed and able to open the schematic **before** you arrive at lab.

1. Download **KiCad 8** (stable) from [kicad.org/download](https://www.kicad.org/download/). Choose the installer for your OS.
2. Run the installer with default options. The full install (including 3D viewer and libraries) is ~3 GB — allow time to download.
3. Open KiCad, then open the 2026 board schematic:
   - File → Open Project → navigate to `Intro-to-CAD-2026/Intro-to-CAD-2026.kicad_pro`
   - Double-click the `.kicad_sch` to open the schematic editor.
4. Spend 10 minutes exploring: find the YD-RP2040 symbol, follow a few net labels to see where they connect, locate the LED circuits.

> **Verify:** You can open `Intro-to-CAD-2026/Intro-to-CAD-2026.kicad_sch` and see the schematic without errors. If KiCad prompts about missing libraries, click **Ignore** — the symbols we need are embedded in the file.

> **Why this matters:** During Labs 1 and 2 you will be asked to find GPIO numbers, op-amp configurations, and peripheral pin assignments directly from the schematic. Students who cannot open KiCad will be blocked on those steps.

Do this to confirm everything is wired together correctly before Lab 1.

1. Clone the class template repo (you'll get the assignment link from your instructor):
   ```bash
   git clone git@github.com:WWU-CPTR480-2026/<your-repo-name>.git
   cd <your-repo-name>
   ```

2. Build the Lab 1 SDK blink:
   ```bash
   mkdir build && cd build
   cmake .. -DPICO_BOARD=pico
   cmake --build . -j4
   ```
   You should see no errors and a file `lab1/lab1_blink.uf2` should appear in `build/lab1/`.

3. If it builds, your toolchain is working. Commit nothing yet — this is just a verification.

> **Common failure:** `PICO_SDK_PATH not set` — means step 1c was skipped or the shell wasn't restarted after editing the profile. Close and reopen your terminal, then retry.

---

#### 1g — Repository Access Policy

Your team repository will be **private** throughout Phase 1 (weeks 1–7). Keep it that way — do not make it public or add collaborators during this phase.

In **week 8**, after review pairs are announced (Monday May 18), your team will add your assigned reviewing team as GitHub collaborators with **Read** access. Instructions:

1. Go to your repo on GitHub → **Settings** → **Collaborators and teams** → **Add people**
2. Add **both members** of the reviewing team by their GitHub usernames
3. Set access level to **Read**
4. Do this by end of day Monday May 18 so reviewers have the full week to study your code

The reviewing team must **not** fork or clone your repo beyond what is needed to read and review. Copying code from a partner team's repo into your own is an academic integrity violation.

You may remove collaborator access after Lab 9 (Tuesday May 26) if you wish.

---

### Part 2 — GitHub Copilot Pro Setup

If you haven't already done so:

1. Verify your student status at [education.github.com/students](https://education.github.com/students). Upload your transcript or enrollment verification. Approval takes 1–3 business days.
2. While waiting, ensure **GitHub Copilot (free tier)** is active in VS Code. You should be able to open Copilot Chat and ask it a question.
3. Once Pro is approved, switch to the Pro plan from your GitHub account settings. Confirm it is active in VS Code (model selector should show more options).

> **Deliverable:** No file needed — your instructor will verify Copilot is working during Lab 2.

---

### Part 3 — Datasheet Summaries

For each of the following components on the 2026 board, use GitHub Copilot Chat (or any AI tool) to help you read and understand the datasheet. Then write your own short summary in your own words — do not just paste AI output. Understanding is the goal.

| Component | Datasheet Source |
|-----------|-----------------|
| **PCM1808** (24-bit stereo audio ADC) | Texas Instruments, search "PCM1808 datasheet" |
| **Si5351a** (programmable clock generator) | Silicon Labs, search "Si5351A datasheet" |
| **QSD (Quadrature Sampling Detector)** | Read the `qsd-sdr.kicad_sch` schematic and the 2026 board schematic; Try to understand how it works using this [reference](https://fweb.wallawalla.edu/~frohro/ClassHandouts/?file=Electronics/A%20Comparison%20of%20Afordable%20Self%20Assembled%20SDR%20Receivers.pdf) |
| **YD-RP2040 v1.3** (Pico-compatible MCU module) | `YD-RP2040_V1.3/YD-RP2040_V1.3.kicad_sch` and `YD-RP2040_V1.3.pdf` in the class repo |

For each component, write a summary in `assignment1/datasheet_notes.md` that covers:
- **What does it do?** (1–2 sentences)
- **How does the host MCU (Pico) communicate with it?** (which protocol: I2C, SPI, I2S, GPIO?)
- **What are the key configuration parameters** we will need to set in firmware? (e.g., sample rate, bit depth, output frequency)
- **What could go wrong?** (1–2 potential failure modes from the datasheet)
- **One thing you found confusing** and how you resolved it (using AI, a colleague, or further reading)

For the YD-RP2040 specifically, your summary should address these **critical differences from the official Pico** (all are visible in the KiCad schematic):
- **Power input:** VSYS is NOT an input on the YD-RP2040 (it is on the official Pico). What pin do you use to supply external power, and what protects against conflict with USB power?
- **Voltage regulator:** What IC provides 3.3 V, and why does it matter for powering peripherals? (Hint: compare the current rating to the official Pico's regulator.)
- **Built-in LED:** There is no simple GPIO-driven onboard LED. What is there instead, and what GPIO and protocol does it use?
- **Pinout:** Is the YD-RP2040 pinout identical to the official Pico? (Check the schematic notes.) What should you always do before connecting YD-RP2040 to a circuit designed for the official Pico?
- **Enhancements:** List the two onboard enhancements (components not on the official Pico) found in the schematic's "Enhancements" section, and which GPIOs they use.

---

### Part 4 — 2026 Board README Skeleton

The 2026 board does not yet have a README. You will create one in Lab 2, filled in with measured data. Your job this week is to build the skeleton so that Lab 2 is efficient.

Create `assignment1/2026_board_README_draft.md` with the following sections. Fill in whatever you can from reading the schematic; leave blanks (`TBD`) where measured data is needed.

```
# WWU 2026 SDR Board

## Overview
[Brief description of what this board is and what it does]

## Hardware Modules
[Table: Module name | IC/Component | Interface | Notes]

## Pin Assignments
[Table: Signal name | Pico GPIO | Direction | Notes]
Fill in as much as you can find from the schematic.

## Power Supply Rails
[Table: Rail name | Nominal voltage | Source | Measured (TBD)]

## Bring-Up Checklist
[ ] Power rails within spec
[ ] Si5351a I2C responsive at 0x60
[ ] OLED responsive on I2C
[ ] MicroSD SPI mount successful
[ ] PCM1808 I2S clocks present
[ ] Debug Stack SWD connection successful

## Known Issues / Notes
[Leave blank for now; fill in after Lab 2]
```

You are expected to have the **Pin Assignments** and **Hardware Modules** tables substantially filled in from the schematic before Lab 2. Use Copilot to help you read the schematic files or ask it to help you parse the KiCad `.sch` files.

---

### Part 5 — Submit Copilot Pro Application

Confirm in your `journal/week1.md` file that you have:
- [ ] Submitted your transcript/enrollment to GitHub Education
- [ ] Copilot (at minimum free tier) is working in VS Code
- [ ] Your GitHub repo for this class is created and accessible to the instructor
- [ ] KiCad 8 installed and the 2026 board schematic opens without errors

---

### Commit Checklist

Your GitHub repo must contain by Monday, April 6 at 11:00 a.m.:

- [ ] `assignment1/datasheet_notes.md` — summaries for PCM1808, Si5351a, QSD, and YD-RP2040
- [ ] `assignment1/2026_board_README_draft.md` — skeleton with modules, pin table, power rail table, checklist
- [ ] `journal/week1.md` — AI reflection (from Lab 1) + Copilot Pro status note + KiCad install confirmed
- [ ] Toolchain verification build succeeds (no need to commit build artifacts)
- [ ] KiCad 8 installed and schematic opens (no commit needed — confirmed in journal)
- [ ] Toolchain verification build succeeds (no need to commit build artifacts)

---

### Grading (this deliverable is part of the Week 1 lab grade)
- Datasheet summaries: Are they in your own words? Do they demonstrate understanding? (not just AI output)
- README skeleton: Is it substantively filled in from the schematic, or mostly blank?
- AI journal: Honest reflection, any length is fine.
