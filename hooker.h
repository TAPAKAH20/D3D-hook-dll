#pragma once
class Hooker
{
public:
	DWORD hook(const LPCTSTR moduleName, const LPCSTR functionName, LPVOID pDest);
	DWORD rehook();
	DWORD unhook();

private:
	LPVOID pDest;
	LPVOID pSource;
	BYTE backupBuff[TRAMPOLINE_LENGTH];
	BYTE tramp[TRAMPOLINE_LENGTH] = { 0xE9, 0x00, 0x00, 0x00, 0x00, 0xC3 };
};

