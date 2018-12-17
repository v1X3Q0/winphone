#include "pch.h"
#include <Windows.h>
#include "init_list.h"


extern unsigned wp_version;

EnumProcessModules_t EnumProcessModules;
EnumDeviceDrivers_t EnumDeviceDrivers;
EnumProcesses_t EnumProcesses;
LoadLibraryW_t LoadLibraryW;
GetModuleFileNameExW_t GetModuleFileNameExW;
GetModuleHandleW_t GetModuleHandleW;
//RtlAdjustPrivilege_t RtlAdjustPrivilege;

int init_funcs(unsigned* kernbase)
{
	unsigned GCP = (unsigned)GetCurrentProcess;											//GetCurrentProcess - 0xEECC1: 1390, 0x7ece1: 1446
	char* check_addr = (char*)(GCP & 0xfffff000);
	DWORD i;
	for (i = (DWORD)check_addr; i > 0; i -= 0x1000)
	{
		DWORD toCmp = 0x00905a4d;
		if (memcmp((void*)i, (void*)&toCmp, 4) == 0)
			break;
	}
	*kernbase = i;
	if ((GCP % 0x1000) == 0xcc1)
		wp_version = WP_15063_1390;
	else if ((GCP % 0x1000) == 0x9b1)
		wp_version = WP_15063_297;
	else if ((GCP % 0x1000) == 0xce1)
		wp_version = WP_15063_1446;
	else
		return -1;

	if (wp_version == WP_15063_1390)
	{
		EnumProcessModules = reinterpret_cast<BOOL(*)(HANDLE, HMODULE*, DWORD, LPDWORD)>(i + 0xe4f61);
		EnumDeviceDrivers = reinterpret_cast<BOOL(*)(LPVOID*, DWORD, LPDWORD)>(i + 0x137dc1);
		EnumProcesses = reinterpret_cast<BOOL(*)(DWORD*, DWORD, LPDWORD)>(i + 0x104741);
		LoadLibraryW = reinterpret_cast<HMODULE(*)(LPCWSTR)>(i + 0xdc041);
		GetModuleFileNameExW = reinterpret_cast<DWORD(*)(HANDLE, HMODULE, LPWSTR, DWORD)>(i + 0xfaaf1);
		//GetProcAddress = reinterpret_cast<FARPROC(*)(HMODULE, LPCSTR)>(kernbase + 0xdbb21);
		GetModuleHandleW = reinterpret_cast<HMODULE(*)(LPCWSTR)>(i + 0xdafd1);
	}
	else if (wp_version == WP_15063_297)
	{
		EnumProcessModules = reinterpret_cast<BOOL(*)(HANDLE, HMODULE*, DWORD, LPDWORD)>(i + 0xE4C51);
		EnumDeviceDrivers = reinterpret_cast<BOOL(*)(LPVOID*, DWORD, LPDWORD)>(i + 0x1378C1);
		EnumProcesses = reinterpret_cast<BOOL(*)(DWORD*, DWORD, LPDWORD)>(i + 0x1040B1);
		LoadLibraryW = reinterpret_cast<HMODULE(*)(LPCWSTR)>(i + 0xDBD21);
		GetModuleFileNameExW = reinterpret_cast<DWORD(*)(HANDLE, HMODULE, LPWSTR, DWORD)>(i + 0xFA7E1);
		//GetProcAddress = reinterpret_cast<FARPROC(*)(HMODULE, LPCSTR)>(kernbase + 0xDB801);
		GetModuleHandleW = reinterpret_cast<HMODULE(*)(LPCWSTR)>(i + 0xDACB1);
	}
	else if (wp_version == WP_15063_1446)
	{
		EnumProcessModules = reinterpret_cast<BOOL(*)(HANDLE, HMODULE*, DWORD, LPDWORD)>(i + 0x000E4F81);
		EnumDeviceDrivers = reinterpret_cast<BOOL(*)(LPVOID*, DWORD, LPDWORD)>(i + 0x00137DD1);
		EnumProcesses = reinterpret_cast<BOOL(*)(DWORD*, DWORD, LPDWORD)>(i + 0x00104761);
		LoadLibraryW = reinterpret_cast<HMODULE(*)(LPCWSTR)>(i + 0x000DC061);
		GetModuleFileNameExW = reinterpret_cast<DWORD(*)(HANDLE, HMODULE, LPWSTR, DWORD)>(i + 0x000FAB11);
		GetModuleHandleW = reinterpret_cast<HMODULE(*)(LPCWSTR)>(i + 0x000DAFF1);
	}

	return 0;
}

//int ntdll_init(unsigned ntdll_base)
//{
//	if (wp_version == WP_15063_297)
//	{
//		RtlAdjustPrivilege = (RtlAdjustPrivilege_t)(ntdll_base + 0x690C1);
//	}
//	else if (wp_version == WP_15063_297)
//	{
//		RtlAdjustPrivilege = (RtlAdjustPrivilege_t)( + 0x690C1);
//	}
//	else if (wp_version == WP_15063_1446)
//	{
//		RtlAdjustPrivilege = (RtlAdjustPrivilege_t)(i + 0x690C1);
//	}
//}