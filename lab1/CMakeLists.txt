cmake_minimum_required(VERSION 3.13)

# Lab 1 — SDK Blink: verifies the C/C++ toolchain is installed and working.
# MicroPython scripts for the rest of Lab 1 live alongside this folder
# and do not need to be part of the CMake build.

add_executable(lab1_blink
    sdk_blink/main.c
)

target_link_libraries(lab1_blink
    pico_stdlib
)

# Route stdio to USB CDC (shows up as a serial port on your PC)
pico_enable_stdio_usb(lab1_blink 1)
pico_enable_stdio_uart(lab1_blink 0)

# Generate .uf2, .bin, .hex, .dis alongside the .elf
pico_add_extra_outputs(lab1_blink)
