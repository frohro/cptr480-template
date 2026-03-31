# CPTR 480 — Spring 2026
## Your Name Here

This is your personal lab repository for CPTR 480 — Programming Embedded and Real-Time Systems.
It is **private** — only you and the instructor can see it.

---

## Quick Start

**New to the repo?** See **[GETTING_STARTED.md](GETTING_STARTED.md)** for a step-by-step guide: cloning (with submodules), installing the SDK/toolchain, building, flashing, debugging, and the weekly workflow.

---

## Capstone Goal
By Week 9, this repository's firmware will turn the [WWU 2026 SDR Board](Intro-to-CAD-2026/) into a
USB Audio Class 2 device tunable from Quisk on a host PC.

---

## Repository Structure

```
/
├── lab1/                ← Lab 1 deliverables (blink, logic analyzer captures, MicroPython scripts)
├── lab2/                ← Lab 2 deliverables (board validation, README)
├── lab3/ ...            ← Added each week
├── assignment1/         ← Assignment 1 deliverables
├── assignment2/         ← Assignment 2 deliverables
├── journal/             ← Weekly AI reflection journals
├── Intro-to-CAD-2026/  ← WWU 2026 SDR board schematic & PCB files (submodule)
├── Pico_Debug_Stack/   ← Debug probe hardware files (submodule)
├── logicanalyzer/      ← Logic analyzer gateware (submodule)
├── yapicoprobe/        ← Yapicoprobe firmware (submodule)
├── Labs/                ← Lab instruction documents
├── Assignments/         ← Assignment documents
├── 2026_board_README.md ← Board documentation (completed by end of Week 2)
├── CPTR-480_Schedule.md ← Course schedule & due dates
├── CMakeLists.txt       ← Top-level build — uncomment subdirs as you progress
└── pico_sdk_import.cmake
```

---

## Build Setup

### Prerequisites
Install the following on your laptop:

| Tool | macOS | Linux (Ubuntu/Debian) | Windows |
|------|-------|----------------------|---------|
| Pico SDK | See [Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) Ch. 2 | Same | Same (use WSL2) |
| `arm-none-eabi-gcc` | `brew install --cask gcc-arm-embedded` | `sudo apt install gcc-arm-none-eabi` | Install via Getting Started Guide |
| `cmake` | `brew install cmake` | `sudo apt install cmake` | Download from cmake.org |
| `ninja` | `brew install ninja` | `sudo apt install ninja-build` | Download from github.com/ninja-build |
| `openocd` (≥ 0.12) | `brew install openocd` | `sudo apt install openocd` | Build from source or use package |

### Set `PICO_SDK_PATH`
Add to your shell profile (`~/.bashrc`, `~/.zshrc`, or Windows Environment Variables):
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
```

### Build
```bash
mkdir build && cd build
cmake .. -DPICO_BOARD=pico
make -j4        # or: ninja
```
The firmware `.uf2` file appears in `build/lab1/` etc.

### Flash
Hold BOOTSEL on the Pico, plug USB — a drive called `RPI-RP2` appears.
Drag and drop the `.uf2` file onto it. Or use `picotool` / the debug probe.

### Debug (VS Code)
1. Connect the Pico Debug Stack via USB
2. Open this folder in VS Code
3. Select the CMake target in the status bar
4. Press **F5** → "Pico Debug (Yapicoprobe / CMSIS-DAP)"

---

## Grading
- **50%** Weekly lab deliverables (GitHub commits, due Tuesday 2:00 p.m. the week after lab)
- **20%** Homework assignments (GitHub commits, due Monday 11:00 a.m.)
- **15%** AI reflection journal (`journal/weekN.md`)
- **15%** Final demo + presentation

See [CPTR-480_Schedule.md](CPTR-480_Schedule.md) for all due dates.
