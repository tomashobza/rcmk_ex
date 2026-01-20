#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>

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
    std::cout << "Address: 0x" << std::hex << address << std::endl;
    
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

    // TODO: ReadProcessMemory

    CloseHandle(process);
    return 0;
}