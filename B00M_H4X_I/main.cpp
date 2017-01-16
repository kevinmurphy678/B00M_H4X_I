//BOOM H4X INTERNAL 0.00001
//DLL MAIN
#include <Windows.h>
#include <stdio.h>
#include <Hookers.h>
void Start(HMODULE hModule)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CONOUT$", "w", stdout);
	printf_s("B00M H4X INTERNAL");
	SourceEngine::moduleFFFF = hModule;
	SourceEngine::Init();
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Start, hModule, NULL, NULL);
	}
	else
	if (dwReason == DLL_PROCESS_DETACH)
	{
		SourceEngine::Unload();
		FreeConsole();
	}

	return TRUE;
}