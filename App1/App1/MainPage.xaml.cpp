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

DWORD kernbase;
int init();
void create_file_sample(Platform::String^ tar_filename, unsigned char* out_buf, size_t out_size);
int dump_kern32(DWORD kernbase);
std::string GetLastErrorAsString();
BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
);

unsigned wp_version;
enum { WP_15063_1390, WP_15063_1446 };


typedef BOOL (*EnumProcessModules_t)(
	HANDLE  hProcess,
	HMODULE *lphModule,
	DWORD   cb,
	LPDWORD lpcbNeeded
);
EnumProcessModules_t EnumProcessModules_f;

typedef BOOL (*EnumDeviceDrivers_t)(
	LPVOID  *lpImageBase,
	DWORD   cb,
	LPDWORD lpcbNeeded
);
EnumDeviceDrivers_t EnumDeviceDrivers_f;

typedef BOOL (*EnumProcesses_t)(
	DWORD   *lpidProcess,
	DWORD   cb,
	LPDWORD lpcbNeeded
);
EnumProcesses_t EnumProcesses_f;

typedef HMODULE (*LoadLibraryW_t)(
	LPCWSTR lpLibFileName
);
LoadLibraryW_t LoadLibraryW_f;

typedef DWORD (*GetModuleFileNameExW_t)(
	HANDLE  hProcess,
	HMODULE hModule,
	LPWSTR  lpFilename,
	DWORD   nSize
);
GetModuleFileNameExW_t GetModuleFileNameExW_f;

typedef FARPROC (*GetProcAddress_t)(
	HMODULE hModule,
	LPCSTR  lpProcName
);
GetProcAddress_t GetProcAddress_f;

typedef HMODULE (*GetModuleHandleW_t)(
	LPCWSTR lpModuleName
);
GetModuleHandleW_t GetModuleHandleW_f;

typedef BOOL (*LookupPrivilegeValueW_t)(
	LPCWSTR lpSystemName,
	LPCWSTR lpName,
	PLUID   lpLuid
);
LookupPrivilegeValueW_t LookupPrivilegeValueW_f;

MainPage::MainPage()
{
	int check_init = init();
	if (check_init == -1)
		return;
	else if (check_init == TRUE)
		goto safe_resume;
	//GetProcessInformation(0, 0, 0  0);
	//char lpbuf[200];
	//RtlZeroMemory(lpbuf, 200);
	//GetCurrentDirectory(200, (LPWSTR)lpbuf);											//Works
	//LPCWSTR new_str = L"C:\\Windows\\system32\\KernelBase.dll";
	//LPCWSTR new_str2 = L"C:\\PROGRAMS\\APPCORNER\\AppCorner.dll";
	//HANDLE new_file = CreateFile2(new_str2, GENERIC_READ, 0, OPEN_EXISTING, 0);			//permission denied
	/* = GetLastError();*/
	//HMODULE kbase = LoadPackagedLibrary(L"ntdll.dll", 0);								//doesn't exists
	//if (kbase == 0)
	//{
	//	return;
	//}
	DWORD i;
	LPVOID drivers_tot = VirtualAlloc(NULL, 500, MEM_COMMIT, PAGE_READWRITE);
	DWORD* procs_tot = (DWORD*)VirtualAlloc(NULL, 500, MEM_COMMIT, PAGE_READWRITE);
	DWORD error_new;
	HANDLE pHand = GetCurrentProcess();
	error_new = GetLastError();
	DWORD pid = GetCurrentProcessId();
	HANDLE opid = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	HMODULE hMods[1024];
	DWORD cbNeeded;
	if (!EnumProcessModules_f(opid, hMods, sizeof(hMods), &cbNeeded))
		return;
	HANDLE Advapi32 = LoadLibraryW_f(L"Advapi32.dll");
	for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
	{
		TCHAR szModName[MAX_PATH];

		// Get the full path to the module's file.

		if (GetModuleFileNameExW_f(pHand, hMods[i], szModName,
			sizeof(szModName) / sizeof(TCHAR)))
		{
			// Print the module name and handle value.
			printf("yes\n");
			//_tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
		}
	}
	LookupPrivilegeValueW_f = (LookupPrivilegeValueW_t)GetProcAddress_f(
		GetModuleHandleW_f(L"Advapi32.dll"),
		"LookupPrivilegeValueW");
	LUID p_in_q;
	memset(&p_in_q, 0, sizeof(LUID));
	DWORD size_priv = 100;
	void* rec_name = VirtualAlloc(NULL, size_priv, MEM_COMMIT, PAGE_READWRITE);
	KEY_CREATE_LINK;
	LookupPrivilegeValueW_f(0, TEXT("SeBackupPrivilege"), &p_in_q);
	memset(&p_in_q, 0, sizeof(LUID));
	p_in_q.LowPart = 0x17;
	HANDLE cur_token;
	if (OpenProcessToken(opid, 0x18, &cur_token))
	{
		SetPrivilege(cur_token, TEXT("SeDebugPrivilege"), TRUE);
	}
	LookupPrivilegeName(NULL, &p_in_q, (LPWSTR)rec_name, &size_priv);
	//DWORD retbytes;
	//if (!EnumProcesses_f(procs_tot, 500, &retbytes))						//didn't work...?
	//	return;
	//HANDLE prn_n = 0;
	//for (i = 0; i < (retbytes / sizeof(DWORD)); i++)
	//{
	//	if ((procs_tot[i] != 0) && (procs_tot[i] != pid))
	//	{
	//		prn_n = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procs_tot[i]);
	//		break;
	//	}
	//}
	HANDLE new_file = CreateFile2(L"new_file", GENERIC_READ | GENERIC_WRITE, 0, CREATE_ALWAYS, 0);
	if (new_file == INVALID_HANDLE_VALUE)
		return;
	HANDLE ws2_base = LoadLibraryW_f(L"Ws2_32.dll");
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

int init()
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
	kernbase = i;
	if ((GCP % 0x1000) == 0xcc1)
		wp_version = WP_15063_1390;
	else if ((GCP % 0x1000) == 0xce1)
	{
		return dump_kern32(kernbase);
	}
	else
		return -1;
		
	if (wp_version == WP_15063_1390)
	{
		EnumProcessModules_f = (EnumProcessModules_t)(kernbase + 0xe4f61);
		EnumDeviceDrivers_f = (EnumDeviceDrivers_t)(kernbase + 0x137dc1);
		EnumProcesses_f = (EnumProcesses_t)(kernbase + 0x104741);
		LoadLibraryW_f = (LoadLibraryW_t)(kernbase + 0xdc041);
		GetModuleFileNameExW_f = (GetModuleFileNameExW_t)(kernbase + 0xfaaf1);
		GetProcAddress_f = (GetProcAddress_t)(kernbase + 0xdbb21);
		GetModuleHandleW_f = (GetModuleHandleW_t)(kernbase + 0xdafd1);
	}
}

BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		printf("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

IBuffer^ GetBufferFromString(String^ str)
{
	InMemoryRandomAccessStream^ memoryStream = ref new InMemoryRandomAccessStream();
	DataWriter^ dataWriter = ref new DataWriter(memoryStream);
	dataWriter->WriteString(str);
	return dataWriter->DetachBuffer();
}

void create_file_sample(Platform::String^ tar_filename, unsigned char* out_buf, size_t out_size)
{
	//auto createFileTask = create_task(localFolder->CreateFileAsync(desiredName, Windows::Storage::CreationCollisionOption::FailIfExists));
	//createFileTask.then([](StorageFile^ newFile)
	//{
	//	//Do something with the new file.
	//});

	//create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary))
	//	.then([this](StorageFolder^ picturesFolder)
	//{
	//	return picturesFolder->CreateFileAsync(rootPage->Filename, CreationCollisionOption::ReplaceExisting);
	//}).then([this](task<StorageFile^> task)
	//{
	//	//do something with file
	//});
	Platform::Array<unsigned char>^ data_t = ref new Platform::Array<unsigned char>(out_buf, out_size);

	create_task(KnownFolders::GetFolderForUserAsync(nullptr /* current user */, KnownFolderId::PicturesLibrary))
		.then([tar_filename](StorageFolder^ picturesFolder)
	{
		return picturesFolder->CreateFileAsync(tar_filename, CreationCollisionOption::ReplaceExisting);
	}).then([out_buf, out_size, data_t](task<StorageFile^> task_file)
	{
		////do something with file
		//create_task(FileIO::WriteBytesAsync(task_file.get(), data_t)).then([](task<void> task_write_check)
		//{
		//	try
		//	{
		//		task_write_check.get();
		//	}
		//	catch (COMException^ ex)
		//	{
		//		printf("%s", ex);
		//	}
		//});
		printf("useless");
	});

}

int dump_kern32(DWORD kernbase)
{
	BOOL bErrorFlag;
	DWORD bWrit;
	wp_version = WP_15063_1446;
	char* kernbase_p = (char*)kernbase;
	unsigned short num_sec = *(unsigned short*)&kernbase_p[0x106];
	DWORD image_size = *(DWORD*)&kernbase_p[0x150];

	void* kern32_filename = L"kern32.dll.raw";
	DWORD fname_tar = wcslen((wchar_t*)kern32_filename);
	LPCWSTR k32_path_const = L"C:\\Data\\Users\\Public\\Recent\\";
	size_t target_path_len = wcslen(k32_path_const);

	char* kern32_path[0x200];
	RtlZeroMemory(kern32_path, 0x200);
	RtlCopyMemory(kern32_path, k32_path_const, target_path_len * sizeof(wchar_t));
	//char lpbuf[0x500];
	//RtlZeroMemory(lpbuf, 0x500);
	//GetCurrentDirectory(0x500, (LPWSTR)lpbuf);											//Works
	//DWORD cwd_len = wcslen((LPWSTR)lpbuf);
	//RtlCopyMemory(lpbuf + cwd_len * sizeof(wchar_t),
	//	L"\\", 2);
	//cwd_len++;
	void* destination_memcpy = (void*)((DWORD)kern32_path + (DWORD)target_path_len * 2);
	RtlCopyMemory(destination_memcpy, (void*)kern32_filename, fname_tar * 2);

	//std::ofstream new_file_cpp("C:\\Data\\Users\\Public\\Documents\\kern32.dll.raw", std::ios::out |
	//	std::ios::binary);
	//new_file_cpp.write(kernbase_p, image_size);
	//new_file_cpp.close();

	//HANDLE new_file = CreateFile2((LPCWSTR)kern32_path,
	//	GENERIC_WRITE, 0, OPEN_ALWAYS, 0);			//permission denied
	//if (new_file == INVALID_HANDLE_VALUE)
	//	GetLastErrorAsString();
	//bErrorFlag = WriteFile(
	//	new_file,           // open file handle
	//	kernbase_p,      // start of data to write
	//	image_size,  // number of bytes to write
	//	&bWrit, // number of bytes that were written
	//	NULL);            // no overlapped structure
	//CloseHandle(new_file);

	Platform::String^ target_file_uwp(L"kern32.dll.raw");
	create_file_sample(target_file_uwp, (unsigned char*)kernbase_p, 0x200);
	return TRUE;
}

std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}
