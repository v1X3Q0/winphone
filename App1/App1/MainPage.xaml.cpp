//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <winternl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ppltasks.h>

#include "init_list.h"
#include "file_open.h"
#include "enum_directory.h"

#include <vector>

using namespace App1;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::System::Threading::Core;
using namespace concurrency;


// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

void create_file_sample(Platform::String^ tar_filename, unsigned char* out_buf, size_t out_size);
void enumMods(HANDLE new_id);
void freeMods();
void enumProcs(std::vector<int> *known_pids);
void openWin32sys();
std::string GetLastErrorAsString();
BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
);
void force_priv();

unsigned wp_version;
unsigned kernbase;


extern EnumProcessModules_t EnumProcessModules;
extern EnumDeviceDrivers_t EnumDeviceDrivers;
extern EnumProcesses_t EnumProcesses;
extern LoadLibraryW_t LoadLibraryW;
extern GetModuleFileNameExW_t GetModuleFileNameExW;
extern GetModuleHandleW_t GetModuleHandleW;

MainPage::MainPage()
{
	std::vector<int> valid_pids, known_pids;
	int check_init = init_funcs(&kernbase);
	enumMods((HANDLE)-1);
	freeMods();
	HANDLE newLIb = LoadLibraryW(L"Chakra.dll");
	enumMods((HANDLE)-1);
	if (check_init == -1)
		return;
	else if (check_init == TRUE)
		goto safe_resume;

	auto PathFileExistsW =
		(PathFileExistsW_t)(GetProcAddress(GetModuleHandleW(L"shlwapi.dll"),
			"PathFileExistsW"));
	auto CreateProcessA =
		(CreateProcessA_t)(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
			"CreateProcessA"));
	HANDLE newFile = 
		CreateFile2(L"C:\\Data\\Users\\Public\\Downloads\\hw.dat",
		GENERIC_ALL, 0, OPEN_ALWAYS, 0);
	if (newFile == INVALID_HANDLE_VALUE)
		GetLastErrorAsString();
	//create_file_sample(L"new_file.txt", 0, 0, false);
	wchar_t cwd[0x100];
	GetCurrentDirectoryW(0x100, cwd);
	enum_directory(cwd);
	if (PathFileExistsW(L"C:\\Data\\Users\\Public\\Downloads\\hello_world.exe"))
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// Start the child process. 
		if (!CreateProcessA(NULL,   // No module name (use command line)
			(LPSTR)"C:\\Data\\Users\\Public\\hello_world.exe",        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi)           // Pointer to PROCESS_INFORMATION structure
			)
		{
			GetLastErrorAsString();
			printf("CreateProcess failed (%d).\n", GetLastError());
			return;
		}

		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);


	}
	else
	{
		printf("%s", GetLastErrorAsString());
	}
	//dump_kern32(kernbase, false, false);
	//OutputDebugStringW(L"killin it\n");
	//enumProcs(&known_pids);
	openWin32sys();
	enumMods((HANDLE)-1);
	/*for (unsigned j = 0; j < valid_pids.size(); j++)
		enumMods((HANDLE)valid_pids[j]);
*/
	LPVOID drivers_tot = VirtualAlloc(NULL, 500, MEM_COMMIT, PAGE_READWRITE);
	DWORD error_new;
	HANDLE pHand = GetCurrentProcess();
	error_new = GetLastError();
	LUID p_in_q;
	memset(&p_in_q, 0, sizeof(LUID));
	DWORD size_priv = 100;
	void* rec_name = VirtualAlloc(NULL, size_priv, MEM_COMMIT, PAGE_READWRITE);
	KEY_CREATE_LINK;
	LookupPrivilegeValueW(0, TEXT("SeBackupPrivilege"), &p_in_q);
	memset(&p_in_q, 0, sizeof(LUID));
	p_in_q.LowPart = 0x17;
	HANDLE cur_token;
	if (OpenProcessToken(pHand, 0x18, &cur_token))
	{
		SetPrivilege(cur_token, TEXT("SeDebugPrivilege"), TRUE);
	}
	LookupPrivilegeName(NULL, &p_in_q, (LPWSTR)rec_name, &size_priv);

	HANDLE new_file = CreateFile2(L"new_file", GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS, 0);
	if (new_file == INVALID_HANDLE_VALUE)
		return;
	HANDLE ws2_base = LoadLibraryW(L"Ws2_32.dll");
	if (ws2_base == 0)
		return;
	error_new = GetLastError();
	//if (!EnumDeviceDrivers_f((LPVOID*)&drivers_tot, 500, &retbytes))		//permission denied
//{
//	error_new = GetLastError();
//	return;
//}
safe_resume:
	InitializeComponent();
}

IBuffer^ GetBufferFromString(String^ str)
{
	InMemoryRandomAccessStream^ memoryStream = ref new InMemoryRandomAccessStream();
	DataWriter^ dataWriter = ref new DataWriter(memoryStream);
	dataWriter->WriteString(str);
	return dataWriter->DetachBuffer();
}

void openWin32sys()
{
	WCHAR               szBuffer[MAX_PATH * 2];
	auto GetSystemDirectory =
		reinterpret_cast<UINT(*)(LPWSTR, UINT)>(GetProcAddress(GetModuleHandleW(L"KernelBase.dll"),
			"GetSystemDirectoryW"));
	RtlSecureZeroMemory(szBuffer, sizeof(szBuffer));
	if (!GetSystemDirectory(szBuffer, MAX_PATH))
		return;
	wcscat_s(szBuffer, TEXT("\\win32k.sys"));
	auto KernelImage = (ULONG_PTR)LoadLibraryW(szBuffer);
	if (KernelImage == 0)
		return;

}

void enumProcs(std::vector<int> *known_pids)
{
	DWORD retbytes, i;
	DWORD pid = GetCurrentProcessId();
	HANDLE opid = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	DWORD* procs_tot = (DWORD*)VirtualAlloc(NULL, 500, MEM_COMMIT, PAGE_READWRITE);
	if (!EnumProcesses(procs_tot, 500, &retbytes))						//didn't work...?
		return;
	HANDLE prn_n = 0;
	for (i = 0; i < (retbytes / sizeof(DWORD)); i++)
	{
		if ((procs_tot[i] != 0))
		{
			//prn_n = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procs_tot[i]);
			known_pids->push_back(procs_tot[i]);
		}
	}
}

void enumMods(HANDLE new_id)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;
	DWORD i, pid = GetCurrentProcessId();
	HANDLE opid;
	if (new_id != (HANDLE)(-1))
		opid = new_id;
	else
		opid = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!EnumProcessModules(opid, hMods, sizeof(hMods), &cbNeeded))
		return;
	//HANDLE Advapi32 = LoadLibraryW(L"Advapi32.dll");
	for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
	{
		TCHAR szModName[MAX_PATH];

		// Get the full path to the module's file.

		if (GetModuleFileNameExW(opid, hMods[i], szModName,
			sizeof(szModName) / sizeof(TCHAR)))
		{
			printf("yes\n");
		}
	}
}

void freeMods()
{
	HMODULE hMods[1024];
	DWORD cbNeeded;
	DWORD i, pid = GetCurrentProcessId();
	HANDLE opid;
	BOOL freeFail = FALSE;
	opid = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!EnumProcessModules(opid, hMods, sizeof(hMods), &cbNeeded))
		return;
	//HANDLE Advapi32 = LoadLibraryW(L"Advapi32.dll");
	for (i = 8; i < (cbNeeded / sizeof(HMODULE)); i++)
	{
		//TCHAR szModName[MAX_PATH];

		//// Get the full path to the module's file.

		//if (GetModuleFileNameExW(opid, hMods[i], szModName,
		//	sizeof(szModName) / sizeof(TCHAR)))
		//{
		//	printf("yes\n");
		//}
		if (i == 26)
			continue;
		freeFail = FreeLibrary(hMods[i]);
		if (freeFail == FALSE)
			return;
	}

}

void bruteforcePID(std::vector<int> *valid_pids)
{
	DWORD pid = GetCurrentProcessId();
	for (int j = 0; j < 20000; j++)
	{
		HANDLE opid = OpenProcess(PROCESS_ALL_ACCESS, FALSE, j);
		if (opid == 0)
			continue;
		else if (j == pid)
			continue;
		else
			valid_pids->push_back((int)opid);
	}
}

void force_priv()
{
	ULONG c;
	BOOLEAN bWasEnabled;
	NTSTATUS new_stat;
	auto RtlAdjustPrivilege =
		(RtlAdjustPrivilege_t)(GetProcAddress(GetModuleHandleW(L"ntdll.dll"),
			"RtlAdjustPrivilege"));
	for (c = SE_MIN_WELL_KNOWN_PRIVILEGE; c <= SE_MAX_WELL_KNOWN_PRIVILEGE; c++) {
		new_stat = RtlAdjustPrivilege(c, TRUE, FALSE, &bWasEnabled);
		if (new_stat == 0)
			printf("goodshit");
	}
}