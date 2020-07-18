#include <cstdio>
#include <conio.h>
#include <iostream>
#include <string>
#include <Windows.h>

// offsets made on 18.07.2020
// if you want to keep using this tool, better make sigs for them before the next update
// im too lazy to do it right now
static const __int64 g_settingsOffset = 0x1474B54E8;
static const __int64 g_updateSettingsFunc = 0x142A8D7A0;

//vtable 0x1439CD580 size: 0x400
//probably contains all graphics settings dunno
struct ResolutionSettings
{
	char Pad000[0x10];					// +0x00
	unsigned int width;					// +0x10
	unsigned int height;				// +0x14
	char Pad018[0x4];					// +0x18
	unsigned int windowMode;			// +0x1C
	unsigned int windowRectTop;			// +0x20
	unsigned int windowRectLeft;		// +0x24
	unsigned int windowRectRight;		// +0x28
	unsigned int windowRectBottom;		// +0x2C
	char Pad030[0x3D0];					// +0x30
};

void printHelp()
{
	system("cls");

	__int64 settingsObj = *(__int64*)g_settingsOffset;
	ResolutionSettings* pResolutionSettings = (ResolutionSettings*)(settingsObj + 0x260);

	std::cout << "Current resolution: " << pResolutionSettings->width << " x " << pResolutionSettings->height << std::endl;
	std::cout << "Press INSERT to set resolution" << std::endl;
}

void main()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	printHelp();

	while(true)
	{
		if(GetAsyncKeyState(VK_INSERT) & 0x8000)
		{
			system("cls");

			unsigned int width, height;

			std::cout << "Set width: ";
			std::cin >> width;

			system("cls");

			std::cout << "Set height: ";
			std::cin >> height;

			system("cls");

			std::cout << "Confirm resolution (Y)es/(N)o" << std::endl;
			std::cout << width << " x " << height << std::endl;

			bool confirmed = false;
			while(true)
			{
				int ch = getchar();

				if(ch == 'y')
				{
					confirmed = true;
					break;
				}
				else if(ch == 'n')
				{
					confirmed = false;
					break;
				}

				Sleep(10);
			}

			if(confirmed)
			{
				// Bring game window into focus before changing resolution, else Steam screenshot function breaks
				HWND hwnd = FindWindowA(0, "Death Stranding");
				if (hwnd == 0)
					hwnd = FindWindowA("PIGS_MainWnd", 0);

				SetForegroundWindow(hwnd);
				SetActiveWindow(hwnd);

				__int64 settingsObj = *(__int64*)g_settingsOffset;
				ResolutionSettings* pResolutionSettings = (ResolutionSettings*)(settingsObj + 0x260);

				pResolutionSettings->width = width;
				pResolutionSettings->height = height;
				
				// Place window in top left corner of the screen
				pResolutionSettings->windowRectTop = pResolutionSettings->windowRectLeft = 0;

				// Set window size to match render resolution
				pResolutionSettings->windowRectRight = width;
				pResolutionSettings->windowRectBottom = height;

				typedef void(__fastcall* tUpdateSettings)(__int64 a1, __int64 a2);
				tUpdateSettings UpdateSettings = (tUpdateSettings)0x142A8D7A0;

				UpdateSettings((__int64)pResolutionSettings, 0x4B);
			}

			printHelp();
		}

		Sleep(10);
	}

}

DWORD WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
	if (_Reason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)main, 0, 0, 0);
	}

	return TRUE;
}