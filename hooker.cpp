#include "pch.h"
#include "hooker.h"


// hook function
DWORD Hooker::hook(const LPCTSTR moduleName, const LPCSTR functionName, LPVOID pDestination) {
    pDest = pDestination;

    HMODULE hModule = GetModuleHandle(moduleName);
    if (hModule == NULL) {
        MessageBox(NULL, _T("Error"), _T("Failes to find module"), MB_OK);
        return 1;
    }


    pSource = GetProcAddress(hModule, functionName);

    if (pSource == nullptr) {
        MessageBox(NULL, _T("Error"), _T("Failes to find function"), MB_OK);
        return 1;
    }

    DWORD jmpLen = (DWORD)pDest - (DWORD)pSource - 5; // calculate jump offset
    memcpy(tramp + 1, &jmpLen, sizeof(DWORD)); // write jump offset into trampoline
    memcpy(backupBuff, pSource, TRAMPOLINE_LENGTH); // save beginig of the function

    DWORD oldProtection = 0;
    if (VirtualProtect(pSource, TRAMPOLINE_LENGTH, PAGE_EXECUTE_READWRITE, &oldProtection) == 0) {// override protection
        MessageBox(NULL, _T("Error"), _T("Failes to override protection "), MB_OK);
        return 1;
    }

    memcpy(pSource, tramp, sizeof(tramp));

    if (VirtualProtect(pSource, TRAMPOLINE_LENGTH, oldProtection, &oldProtection) == 0) {// reset protection
        MessageBox(NULL, _T("Error"), _T("Failes to reset protection "), MB_OK);
        return 1;
    }

    return 0;
}


// retrun function to initial state
DWORD Hooker::unhook() {
    DWORD oldProtection = 0;
    if (VirtualProtect(pSource, TRAMPOLINE_LENGTH, PAGE_EXECUTE_READWRITE, &oldProtection) == 0) {// override protection
        MessageBox(NULL, _T("Error"), _T("Failes to override protection "), MB_OK);
        return 1;
    }

    memcpy(pSource, backupBuff, TRAMPOLINE_LENGTH);

    if (VirtualProtect(pSource, TRAMPOLINE_LENGTH, oldProtection, &oldProtection) == 0) {// reset protection
        MessageBox(NULL, _T("Error"), _T("Failes to reset protection "), MB_OK);
        return 1;
    }

    return 0;
}

// r
DWORD Hooker::rehook() {
    DWORD jmpLen = (DWORD)pDest - (DWORD)pSource - 5; // calculate jump offset
    memcpy(tramp + 1, &jmpLen, sizeof(DWORD)); // write jump offset into trampoline
    memcpy(backupBuff, pSource, TRAMPOLINE_LENGTH); // save beginig of the function

    DWORD oldProtection = 0;
    if (VirtualProtect(pSource, TRAMPOLINE_LENGTH, PAGE_EXECUTE_READWRITE, &oldProtection) == 0) {// override protection
        MessageBox(NULL, _T("Error"), _T("Failes to override protection "), MB_OK);
        return 1;
    }

    memcpy(pSource, tramp, sizeof(tramp));

    if (VirtualProtect(pSource, TRAMPOLINE_LENGTH, oldProtection, &oldProtection) == 0) {// reset protection
        MessageBox(NULL, _T("Error"), _T("Failes to reset protection "), MB_OK);
        return 1;
    }

    return 0;
}