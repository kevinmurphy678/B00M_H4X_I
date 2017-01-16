#include <windows.h>  
#include <tlhelp32.h>  
#include <stdio.h>  
#include <iostream>
#include <Psapi.h>
#include <Shlwapi.h>
/*
	Super shitty  LoadLibraryA Injector ._O
	PART OF THE B00M_H4X
	best hacks n.a
*/
DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

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
int main()
{
	printf_s("Waiting...");
	DWORD procID=0;
	LPVOID mem,load;
	HANDLE proc;
	char dll[MAX_PATH];

	const std::wstring procName = std::wstring(TEXT("csgo.exe"));

	procID = FindProcessId(procName);

	//Get the full path of our .dll 
	//Note, set C++ Working directory to 'DEBUG' folder, where our DLL IS!
	GetFullPathNameA("B00M_H4X_I.dll", MAX_PATH, dll, NULL);

	std::cout << "DLL Location: ";
	std::cout << dll;
	std::cout << '\n';

	std::cout << "Process ID: ";
	std::cout << procID;
	std::cout << '\n';

	proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	procID = FindProcessId(procName);

	proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	if (proc == NULL)
	{
		std::cout << "Process Not Running! ";
		scanf_s("");
		return 0;
	}

	load = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32"), "LoadLibraryA");
	mem = (LPVOID)VirtualAllocEx(proc, NULL, strlen(dll), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(proc, (LPVOID)mem, dll, strlen(dll), NULL);
	HANDLE hThread2 = CreateRemoteThread(proc, NULL, NULL, (LPTHREAD_START_ROUTINE)load, mem, NULL, NULL);
	//WaitForSingleObjectEx(hThread2, INFINITE, FALSE);
	CloseHandle(proc);

	scanf_s(""); //TYPE SOME LETTER OR WORD TO DE-INJECT... 

	HMODULE INJECTEDM = 0;
	HMODULE hMods[0x400];
	DWORD dwNeeded;

	procID = FindProcessId(procName);

	proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	EnumProcessModules(proc, hMods, 0x400, &dwNeeded);

	for (int i = 0; i < (dwNeeded / sizeof(DWORD)); i++)
	{
		char szPath[MAX_PATH] = "";
		GetModuleBaseNameA(proc, hMods[i], szPath, MAX_PATH);
		//PathStripPathA(szPath);

		if (!_stricmp(szPath, "B00M_H4X_I.dll"))
		{
			std::cout << "UNINJECTING";
			INJECTEDM = hMods[i];
		}
	}

	char buf[50] = { 0 };
	LPVOID RemoteString, FreeLibAddy;
	FreeLibAddy = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "FreeLibrary");
	RemoteString = (LPVOID)VirtualAllocEx(proc, NULL, sizeof(INJECTEDM), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	HANDLE hThread = CreateRemoteThread(proc, NULL, NULL, (LPTHREAD_START_ROUTINE)FreeLibAddy, (LPVOID)INJECTEDM, NULL, NULL);
	WaitForSingleObjectEx(hThread, 500, FALSE);
	VirtualFreeEx(proc, RemoteString, sizeof(INJECTEDM), MEM_DECOMMIT);

	CloseHandle(proc);

	return 0;
}