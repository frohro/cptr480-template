cmake_minimum_required(VERSION 3.13)

# Pull in the Pico SDK (must be before project())
include(pico_sdk_import.cmake)

project(cptr480 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# ---------------------------------------------------------------------------
# Add lab subdirectories as you complete each lab.
# Uncomment the next line only when you have a CMakeLists.txt in that folder.
# ---------------------------------------------------------------------------
add_subdirectory(lab1)
# add_subdirectory(lab2)     # Uncomment in Week 2
# add_subdirectory(lab3)     # Uncomment in Week 3
# add_subdirectory(lab4)     # Uncomment in Week 4
# add_subdirectory(lab5)     # Uncomment in Week 5
# add_subdirectory(lab6)     # Uncomment in Week 6
# add_subdirectory(lab7)     # Uncomment in Week 7
# add_subdirectory(lab8)     # Uncomment in Week 8
# add_subdirectory(lab9)     # Uncomment in Week 9
# add_subdirectory(lab10)    # Uncomment in Week 10
