#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#pragma region Globals
DWORD dwMainProcessID = 0x00;

//Return process id 
DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

int main(void) {


	dwMainProcessID = FindProcessId(L"idaq.exe");
	if (dwMainProcessID) {
		std::cout << dwMainProcessID << "\n";
		LPCSTR DllPath = "Paste Dll Path right here!";

		// Open a handle to target process
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwMainProcessID);
		//Cấp phát vùng nhớ
		LPVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
		//Write path tới địa chỉ nhớ vừa được cấp phát bên trên 
		WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath, strlen(DllPath) + 1, 0);
		//Tạo một thread để load dll
		HANDLE hLoadThread = 
			CreateRemoteThread(hProcess, 0, 0, 
			(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"), 
				pDllPath, 0, 0);
		//Để chắc chắn rằng DLL đã được thực thi trước khi windows thực thi các công việc tiếp theo của process
		WaitForSingleObject(hLoadThread, INFINITE);

		std::cout << "Dll path allocated at: " << std::hex << pDllPath << std::endl;
		std::cin.get();

		//Giải phóng bộ nhớ đã được cấp phát cho dll path
		VirtualFreeEx(hProcess, pDllPath, strlen(DllPath) + 1, MEM_RELEASE);
		return 0;
	}
	else {
		return -1;
	}
}
