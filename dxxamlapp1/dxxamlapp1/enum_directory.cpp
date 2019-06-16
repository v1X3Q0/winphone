#include "pch.h"
#include <windows.h>

int enum_directory(CHAR *argv)
{
	WIN32_FIND_DATAA ffd;
	LARGE_INTEGER filesize;
	CHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	// Check that the input path plus 3 is not longer than MAX_PATH.
	// Three characters are for the "\*" plus NULL appended below.

	length_of_arg = strlen(argv/*, MAX_PATH, &length_of_arg*/);

	if (length_of_arg > (MAX_PATH - 3))
	{
		printf("\nDirectory path is too long.\n");
		return (-1);
	}
	printf("\nTarget directory is %s\n\n", argv);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	strcpy_s(szDir, MAX_PATH, argv);
	//strcpy_s(szDir, length_of_arg, argv);
	strcat_s(szDir, MAX_PATH, "\\*");

	// Find the first file in the directory.

	hFind = FindFirstFileA(szDir, &ffd);

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
	} while (FindNextFileA(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("lastfile");
	}

	FindClose(hFind);
	return dwError;
}
