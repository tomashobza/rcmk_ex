RACEMAKE TECHNICAL CHALLENGE - VARIANT A: THE MEMORY HUNTER
============================================================

A proof-of-concept demonstrating external process memory reading on Windows
using WinAPI (ReadProcessMemory).


OVERVIEW
--------
This project consists of two applications:

1. TARGET (target.exe)
   Simulates a racing telemetry source. Holds a Telemetry struct in memory
   and continuously updates it with simulated car data (speed, rpm, gear).
   Prints the memory address of the struct on startup.

2. HACKER (hacker.exe)
   Finds the target process by name, opens a handle to its memory, and
   continuously reads the Telemetry struct from the provided address.
   Demonstrates external memory reading without target cooperation.


FILES
-----
target.cpp      - Target application source
hacker.cpp      - Hacker application source  
build.bat       - Build script
racemake_demo.mp4 - Video demonstration


HOW IT WORKS
------------
1. Target allocates a Telemetry struct on the stack
2. Target prints the struct's memory address and its PID
3. Hacker uses CreateToolhelp32Snapshot to enumerate processes
4. Hacker finds target.exe and calls OpenProcess with PROCESS_VM_READ
5. Hacker calls ReadProcessMemory to read the Telemetry struct
6. Torn read protection: struct has sim_time_start and sim_time_end fields,
   hacker retries if they don't match (indicates partial update)


TELEMETRY STRUCT
----------------
struct Telemetry {
    uint32_t sim_time_start;  // Frame counter (start)
    uint16_t speed;           // km/h
    uint16_t rpm;             // Engine RPM  
    uint8_t gear;             // Current gear
    uint32_t sim_time_end;    // Frame counter (end, must match start)
};


BUILD REQUIREMENTS
------------------
- Zig compiler (used as C++ toolchain)
  Download: https://ziglang.org/download/
  Extract to project folder as: zig-x86_64-windows-0.15.2/

- Windows 10/11 x64


BUILD & RUN
-----------
1. Run build.bat

2. Start target in one terminal:
   > target.exe
   
   Note the printed address, e.g.: Telemetry address: 0x00000012345678

3. Start hacker in another terminal with target name and address:
   > hacker.exe target.exe 00000012345678

4. Hacker will display telemetry data read from target's memory


POSSIBLE IMPROVEMENTS
---------------------
- Pattern scanning to find struct address automatically
- GUI visualization with raylib (attempted, linking issues with Zig)
- Named mutex for proper cross-process synchronization
- Support for real racing simulators (ACC, iRacing, rFactor 2)


AUTHOR
------
Tomas Hobza
January 2026