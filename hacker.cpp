#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <cstdlib>

// struct copied from target.cpp
// irl this would need to be derived from either docs or
// crosschecking the binary values with what the target shows
struct Telemetry {
    uint32_t sim_time_start;
    uint16_t speed;
    uint16_t rpm;
    uint8_t gear;
    uint32_t sim_time_end;
};

// helper to print telemetry data
void printTelemetry(Telemetry telem) {
    std::cout << "====" << std::endl;
    std::cout << "sim_time: " << telem.sim_time_start << std::endl;
    std::cout << "\tspeed: " << telem.speed << std::endl;
    std::cout << "\trpm: " << telem.rpm << std::endl;
    std::cout << "\tgear: " << (int)telem.gear << std::endl;
}

// helper to find a PID by exe name
DWORD findProcess(const char* name) {
    // create a snapshot of all running processes
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    // set the size of process entry
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    // iterate through the processes
    if (Process32First(snapshot, &entry)) {
        do {
            // check if the exe file of the process matches the target name
            if (_stricmp(entry.szExeFile, name) == 0) {
                CloseHandle(snapshot);
                // if so, return the PID of the found process
                return entry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return 0;
}

int main(int argc, char ** argv) {
    // check if all params are provided
    if (argc < 3) {
        std::cout << "Usage: hacker.exe <target> <address>" << std::endl;
        return 1;
    }

    // extract target name from argv
    const char* target = argv[1];
    // extract address from argv 
    uintptr_t address = std::strtoull(argv[2], nullptr, 16);

    std::cout << "Target: " << target << std::endl;
    std::cout << "Address: 0x" << std::hex << address << std::dec << std::endl;
    
    // find the target process
    DWORD pid = findProcess(target);
    if (!pid) {
        std::cout << "Target not found" << std::endl;
        return 1;
    }
    std::cout << "Found target, PID: " << pid << std::endl;

    // get a handle for the target process object for reading it's virtual memory
    HANDLE process = OpenProcess(PROCESS_VM_READ, FALSE, pid);
    if (!process) {
        std::cout << "Failed to open process" << std::endl;
        return 1;
    }

    // prepare the buffer to write the read data into
    Telemetry telem = {};
    SIZE_T bytes_read = 0;

    while (true) {
        // read the process memory at the provided address
        bool read_succ = ReadProcessMemory(process, (LPCVOID)address, &telem, sizeof(Telemetry), &bytes_read);
    
        // check if the whole structure was read
        if (!read_succ || bytes_read != sizeof(Telemetry)) {
            std::cout << "Memory read was unsuccessful. Error: " << GetLastError() << std::endl;
            CloseHandle(process);
            return 1;
        }

        // check sim_time_start == sim_time_end
        if (telem.sim_time_start != telem.sim_time_end) {
            // we read partially updated data
            continue;
        }

        // TODO: only print if sim_time is different from last iteration

        // print the read data
        printTelemetry(telem);

        // faster read freq than the data is updated to not miss anything
        Sleep(50);
    }

    CloseHandle(process);
    return 0;
}