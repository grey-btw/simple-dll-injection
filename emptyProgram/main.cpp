#include <Windows.h>
#include <shellapi.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		MessageBox(0, L"DLL Injected!", L"Successful", MB_ICONINFORMATION);
	return TRUE;
}