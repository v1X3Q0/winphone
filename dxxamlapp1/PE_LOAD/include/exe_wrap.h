#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#define PE_HEAD_PTR 0x3c
#define COFF_OFFSET 0x4
#define OPT_OFFSET (COFF_OFFSET + sizeof(IMAGE_FILE_HEADER))
#define TO_USHORT(addr)(*(unsigned short *)(addr))
#define TO_UINT(addr)(*(unsigned int *)(addr))
#define IMPORT_DESC_OFFSET 0x1



class exe_wrap
{
private:
	void *Lord_Vader;
	size_t Lord_Vader_size;
	IMAGE_FILE_HEADER *coff_head;
	IMAGE_OPTIONAL_HEADER *opt_head;
	IMAGE_SECTION_HEADER *sections, *JEHE;
	IMAGE_THUNK_DATA32 *thunk_i_can;
	IMAGE_IMPORT_DESCRIPTOR *import_descriptors;
	IMAGE_BASE_RELOCATION *relocation_table;

	bool zombie, mutex;
public:
	exe_wrap();
	HANDLE CreateSuspended(std::string);
	int __main();
	template<typename T, typename V>
	T to_T(V *converting)
	{
		return *(T*)(converting);
	}

	union size_file_dll
	{
		DWORD num_byte32;
		size_t file_size_t;
		LARGE_INTEGER file_size_p;
		size_file_dll()
		{
			file_size_t = 0;
		}
	};

	template<typename T>
	T check(T j)
	{
		if (j == 0)
		{
			cout << GetLastError() << endl;
			system("PAUSE");
			exit(EXIT_FAILURE);
		}
		else
			return j;
	}
};

