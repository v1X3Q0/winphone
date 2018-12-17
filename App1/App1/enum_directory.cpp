#include "pch.h"
#include <windows.h>

int enum_directory(wchar_t *argv)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.

	length_of_arg = wcslen(argv/*, MAX_PATH, &length_of_arg*/);

	if (length_of_arg > (MAX_PATH - 3))
	{
		printf("\nDirectory path is too long.\n");
		return (-1);
	}

	printf("\nTarget directory is %s\n\n", argv);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	//wcscpy_s(szDir, length_of_arg, argv);
	RtlZeroMemory(szDir, sizeof(szDir));
	RtlCopyMemory((void*)szDir, (void*)argv, length_of_arg * 2);
	wcscat_s(szDir, MAX_PATH, L"\\*");

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return dwError;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			printf("  %s   <DIR>\n", ffd.cFileName);
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			printf("  %s   %ld bytes\n", ffd.cFileName, filesize.QuadPart);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("lastfile");
	}

	FindClose(hFind);
	return dwError;
}
