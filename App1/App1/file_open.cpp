#include "pch.h"
#include "MainPage.xaml.h"

#include <winternl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ppltasks.h>

#include "file_open.h"
#include "util.h"

using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::System::Threading::Core;
using namespace concurrency;


extern unsigned wp_version;

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

int dump_kern32(DWORD kernbase, bool local, bool remote)
{
	void* kern32_filename = L"kern32.dll.raw";
	char* kernbase_p = (char*)kernbase;
	if (remote == false)
	{
		unsigned short num_sec = *(unsigned short*)&kernbase_p[0x106];
		DWORD image_size = *(DWORD*)&kernbase_p[0x150];
		DWORD bErrorFlag, bWrit;
		DWORD fname_tar = wcslen((wchar_t*)kern32_filename);
		//LPCWSTR k32_path_const = L"C:\\Data\\Users\\Public\\Recent\\";
		LPCWSTR k32_path_const = L"C:\\Windows\\System32\\Tasks\\";
		size_t target_path_len = wcslen(k32_path_const);
		char lpbuf[0x500];

		RtlZeroMemory(lpbuf, 0x500);

		if (local == false)
		{
			RtlCopyMemory(lpbuf, k32_path_const, target_path_len * sizeof(wchar_t));
			//RtlCopyMemory(lpbuf + wcslen((LPWSTR)lpbuf) * sizeof(wchar_t),
			//	L"\\", 2);
			//target_path_len++;
		}
		else if (local == true)
		{
			GetCurrentDirectory(0x500, (LPWSTR)lpbuf);											//Works
		}
		target_path_len = wcslen((LPWSTR)lpbuf) + 1;
		void* destination_memcpy = (void*)((DWORD)lpbuf + (DWORD)target_path_len * 2);
		RtlCopyMemory(destination_memcpy, (void*)kern32_filename, fname_tar * 2);

		/*std::ofstream new_file_cpp("C:\\Data\\Users\\Public\\Documents\\kern32.dll.raw", std::ios::out |
			std::ios::binary);
		new_file_cpp.write(kernbase_p, image_size);
		new_file_cpp.close();
	*/
		HANDLE new_file = CreateFile2((LPCWSTR)lpbuf,
			GENERIC_WRITE, 0, OPEN_ALWAYS, 0);			//permission denied
		if (new_file == INVALID_HANDLE_VALUE)
			GetLastErrorAsString();
		bErrorFlag = WriteFile(
			new_file,           // open file handle
			kernbase_p,      // start of data to write
			image_size,  // number of bytes to write
			&bWrit, // number of bytes that were written
			NULL);            // no overlapped structure
		CloseHandle(new_file);
	}
	else
	{
		Platform::String^ target_file_uwp(L"kern32.dll.raw");
		create_file_sample(target_file_uwp,
			(unsigned char*)kernbase_p, 0x200, false);
	}
	return TRUE;
}

void create_file_sample(Platform::String^ tar_filename,
	unsigned char* out_buf, size_t out_size, bool old_api)
{
	if (old_api == true)
	{
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
	else
	{
		auto createFileTask = create_task(DownloadsFolder::CreateFileAsync(tar_filename));
		createFileTask.then([](StorageFile^ newFile)
		{
			// Process file
		});
	}

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

}
