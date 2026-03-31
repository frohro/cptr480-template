# Getting Started — CPTR 480 Student Repository

This document walks you through **cloning, setting up, building, flashing, and debugging** your CPTR 480 repository from scratch. Follow it top-to-bottom the first time you set up a new machine.

---

## 1. Clone Your Repository

Your repository has **submodules** (board files, debug tools, logic analyzer). You must clone with `--recurse-submodules` or the subdirectories will be empty.

```bash
git clone --recurse-submodules git@github.com:WWU-CPTR480-2026/<your-repo-name>.git
cd <your-repo-name>
```

If you already cloned without `--recurse-submodules`, run this to populate the submodules:

```bash
git submodule update --init --recursive
```

### Submodules in this repo

| Directory | Contents |
|---|---|
| `Intro-to-CAD-2026/` | WWU 2026 SDR board schematic and PCB files (KiCad) |
| `Pico_Debug_Stack/` | Debug probe hardware design files |
| `logicanalyzer/` | PulseView-compatible logic analyzer gateware |
| `yapicoprobe/` | Yapicoprobe CMSIS-DAP firmware |

---

## 2. Install Prerequisites

Install the following tools on your laptop:

### macOS (Homebrew)
```bash
brew install cmake ninja
brew install --cask gcc-arm-embedded
brew install openocd
```

### Linux (Ubuntu / Debian)
```bash
sudo apt update
sudo apt install cmake ninja-build gcc-arm-none-eabi openocd git python3
```

### Windows
Use **WSL2** (Ubuntu) and follow the Linux steps above, or follow Chapter 2 of the [Raspberry Pi Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf).

---

## 3. Install the Pico SDK

The Pico SDK is **not** included in this repo — you install it once per machine.

```bash
# Clone the SDK alongside your repo (or anywhere convenient)
git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git ~/pico-sdk
```

Then add this to your shell profile (`~/.bashrc`, `~/.zshrc`, or Windows WSL2 equivalent):

```bash
export PICO_SDK_PATH=$HOME/pico-sdk
```

Reload your shell (`source ~/.bashrc`) or open a new terminal.

---

## 4. Build Firmware

All lab firmware is built from the top-level `CMakeLists.txt`. Subdirectories are commented out by default — uncomment the one you are working on before building.

```bash
mkdir build && cd build
cmake .. -DPICO_BOARD=pico   # use -DPICO_BOARD=pico2 for Pico 2 / -DPICO_BOARD=pico_w for Pico W
make -j$(nproc)              # or: ninja
```

The compiled firmware for, e.g., Lab 1 appears at `build/lab1/lab1.uf2`.

### Enabling a lab in CMakeLists.txt

Open `CMakeLists.txt` in the repo root and uncomment the line for the lab you are working on:

```cmake
# add_subdirectory(lab1)   ← remove the # to enable
add_subdirectory(lab1)
```

Re-run `cmake ..` after changing `CMakeLists.txt`, then `make`/`ninja` again.

---

## 5. Flash Firmware

### Method A — USB Mass Storage (no debug probe needed)
1. Hold the **BOOTSEL** button on your RP2040 board.
2. Plug USB into your laptop while holding BOOTSEL.
3. A drive named `RPI-RP2` appears.
4. Drag and drop the `.uf2` file onto the drive. The board reboots automatically.

### Method B — SWD via Pico Debug Stack (faster, keeps serial open)
```bash
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg \
        -c "program build/lab1/lab1.elf verify reset exit"
```

### Method C — picotool
```bash
picotool load build/lab1/lab1.uf2 --force
```

---

## 6. Debug with VS Code

This repo is pre-configured for VS Code + the Pico Debug Stack (Yapicoprobe).

1. Install the **Cortex-Debug** extension in VS Code.
2. Connect the Pico Debug Stack via USB.
3. Open this folder in VS Code (`File → Open Folder`).
4. Select the CMake target in the status bar at the bottom.
5. Press **F5** to start a debug session — breakpoints, watchpoints, and the serial monitor all work.

> The launch configuration uses Yapicoprobe (CMSIS-DAP). Make sure the debug probe is connected before pressing F5.

---

## 7. Weekly Workflow

Each week you will:

1. **Pull** any lab/assignment updates the instructor pushed:
   ```bash
   git pull --recurse-submodules
   ```

2. **Work** in the appropriate `labN/` or `assignmentN/` folder.

3. **Commit and push** so the instructor can see your progress:
   ```bash
   git add -A
   git commit -m "lab2: board validation complete, OLED working"
   git push
   ```

4. **Write your AI journal entry** in `journal/weekN.md` — reflect on how you used AI tools (GitHub Copilot, ChatGPT, etc.) during the week.

> **Grading is based on GitHub commits, not D2L uploads.** Push your work before the deadline. See [CPTR-480_Schedule.md](CPTR-480_Schedule.md) for all due dates.

---

## 8. Viewing the 2026 Board Schematic

The board schematic and PCB files live in `Intro-to-CAD-2026/`. To open them:

1. Install [KiCad](https://www.kicad.org/download/) (free, cross-platform).
2. Open `Intro-to-CAD-2026/Intro-to-CAD-2026.kicad_pro` in KiCad.

You will need to read the schematic regularly — particularly the Si5351a LO, PCM1808 ADC, USB, and power sections.

---

## 9. Troubleshooting

| Problem | Fix |
|---|---|
| `submodule` directories are empty | Run `git submodule update --init --recursive` |
| `PICO_SDK_PATH not set` | Add `export PICO_SDK_PATH=...` to your shell profile and reload |
| `arm-none-eabi-gcc not found` | Install the ARM toolchain (Step 2 above) |
| `RPI-RP2` drive doesn't appear | Hold BOOTSEL **before** plugging USB, not after |
| CMake configures but nothing compiles | Check that the lab subdirectory is uncommented in `CMakeLists.txt` |
| OpenOCD says "no device found" | Verify the debug probe USB is connected; try a different cable |
| VS Code can't find the debug config | Make sure you opened the **repo folder** (not a subfolder) in VS Code |

For further help, post in the class discussion channel or bring your setup to office hours.
