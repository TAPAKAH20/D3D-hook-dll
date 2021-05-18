// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"

#define TRAMPOLINE_LENGTH 6



// hook function
DWORD hook(const LPCTSTR moduleName, const LPCSTR functionName, LPVOID pDest, LPVOID& pSource, BYTE(&backupBuff)[TRAMPOLINE_LENGTH]) {
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

    BYTE tramp[TRAMPOLINE_LENGTH] = { 0xE9, 0x00, 0x00, 0x00, 0x00, 0xC3 };

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
DWORD unhook(LPVOID pSource, PBYTE backupBuff) {
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
}


// hook destination for function for D3D11CreateDeviceAndSwapChain
void swapChainHook() {
    MessageBox(NULL, _T("Succsess"), _T("Hook succsessful "), MB_OK);
}


void mainThread() {
    LPVOID pSource = {};
    BYTE backupBuff[TRAMPOLINE_LENGTH];
    if (hook(L"D3D11.dll", "D3D11CreateDeviceAndSwapChain", swapChainHook, pSource, backupBuff) == 0)
    {
        // END key to unload
        while (!GetAsyncKeyState(VK_END));
    }

    unhook(pSource, backupBuff);
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)mainThread, hModule, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:

        break;
    }
    return TRUE;
    
}
