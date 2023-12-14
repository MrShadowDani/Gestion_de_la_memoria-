#include <cstdio>
#include <cstring>
#include <windows.h>

#define SIZE 4096

int main() {
    HANDLE hMapFile;
    char* pBuf;

    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, SIZE, "SharedMemory");
    if (hMapFile == nullptr) {
        printf("Could not create file mapping object (%lu).\n", GetLastError());
        return 1;
    }

    pBuf = reinterpret_cast<char*>(MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SIZE));
    if (pBuf == nullptr) {
        printf("Could not map view of file (%lu).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(nullptr, "child_process.exe", nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        printf("CreateProcess failed (%lu).\n", GetLastError());
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        return 1;
    }

    // Copying string to shared memory
    strcpy_s(pBuf, SIZE, "Hello, child process!");
    WaitForSingleObject(pi.hProcess, INFINITE);

    printf("Child reads: %s\n", pBuf);

    UnmapViewOfFile(pBuf);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hMapFile);

    return 0;
}
