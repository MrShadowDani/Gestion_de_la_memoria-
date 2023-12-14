#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define SIZE 4096

int main() {
    HANDLE hMapFile;
    LPCTSTR pBuf;

    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SIZE, "SharedMemory");
    if (hMapFile == NULL) {
        printf("Could not create file mapping object (%d).\n", GetLastError());
        return 1;
    }

    pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SIZE);
    if (pBuf == NULL) {
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        printf("Error occurred during fork.\n");
        CloseHandle(hMapFile);
        UnmapViewOfFile(pBuf);
        return 1;
    } else if (pid == 0) {
        printf("Child reads: %s\n", pBuf);
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        return 0;
    } else {
        strcpy(pBuf, "Hello, child process!");
        WaitForSingleObject(pid, INFINITE);
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        return 0;
    }
}
