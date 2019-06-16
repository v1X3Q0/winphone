//load an executable
#include "../include/exe_wrap.h"
using namespace std;

exe_wrap::exe_wrap()
{
	zombie = 0;
	mutex = 0;
}

HANDLE exe_wrap::CreateSuspended(string arg111)
{
	if (mutex == 1)
	{
		VirtualFree(Lord_Vader, Lord_Vader_size, MEM_RELEASE);
		Lord_Vader_size = 0;
		Lord_Vader = nullptr;
		coff_head = nullptr;
		opt_head = nullptr;
		sections = nullptr;
		JEHE = nullptr;
		thunk_i_can = nullptr;
		import_descriptors = nullptr;
		relocation_table = nullptr;
	}
	const string file_name = arg111;
	HANDLE parttwo = check<HANDLE>(CreateFile((LPCSTR)file_name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
	size_file_dll size_dll;
	DWORD size_read;
	GetFileSizeEx(parttwo, &(size_dll.file_size_p));
	char* file_data = (char*)malloc(size_dll.file_size_t);
	check<BOOL>(ReadFile(parttwo, (void*)file_data, size_dll.num_byte32, &size_read, NULL));
	check<BOOL>((to_T<unsigned short>(&file_data[0]) == IMAGE_DOS_SIGNATURE));
	unsigned int pe_base = to_T<unsigned int, char>(&file_data[0x3C]);
	check<BOOL>(*(unsigned int*)(&(file_data[pe_base])) == IMAGE_NT_SIGNATURE);
	int yyy = sizeof(IMAGE_OPTIONAL_HEADER);
	//Get headers
	coff_head = (IMAGE_FILE_HEADER *)&file_data[pe_base + COFF_OFFSET];
	opt_head = (IMAGE_OPTIONAL_HEADER *)&file_data[pe_base + OPT_OFFSET];

	const unsigned int section_table = pe_base + sizeof(IMAGE_FILE_HEADER) + 4 + coff_head->SizeOfOptionalHeader;
	const unsigned int num_of_sections = coff_head->NumberOfSections;
	sections = JEHE = (IMAGE_SECTION_HEADER *)&file_data[section_table];
	size_t headers_size = opt_head->SizeOfHeaders;
	Lord_Vader_size = opt_head->SizeOfImage;

	Lord_Vader = VirtualAlloc(/*(LPVOID)(0x10000000)*/NULL, Lord_Vader_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	memcpy(Lord_Vader, file_data, headers_size);
	for (DWORD i = 0; i < num_of_sections; i++, sections++)
		memcpy((char*)(Lord_Vader)+sections->VirtualAddress, &file_data[(unsigned int)(sections->PointerToRawData)], sections->SizeOfRawData);

	import_descriptors = (IMAGE_IMPORT_DESCRIPTOR *)((unsigned int)Lord_Vader + opt_head->DataDirectory[IMPORT_DESC_OFFSET].VirtualAddress);
	int num_of_imports = opt_head->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size / sizeof(IMAGE_IMPORT_DESCRIPTOR) - 1;
	thunk_i_can = (IMAGE_THUNK_DATA32*)(opt_head->DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress + (unsigned int)(Lord_Vader));

	vector<string> names;
	int num_thunks = 0;
	for (int i = 0; i < num_of_imports; i++, import_descriptors++)
	{
		names.push_back((char*)((unsigned int)(import_descriptors->Name + (unsigned int)Lord_Vader)));
		//cout << names[i] << endl;
		HMODULE temp_Lib = check<HMODULE>(LoadLibrary(names[i].c_str()));
		IMAGE_THUNK_DATA32 *ogT = (IMAGE_THUNK_DATA32*)(import_descriptors->OriginalFirstThunk + (DWORD)Lord_Vader);
		IMAGE_THUNK_DATA32 *ngT = (IMAGE_THUNK_DATA32*)(import_descriptors->FirstThunk + (DWORD)Lord_Vader);
		while (ogT->u1.AddressOfData != 0)
		{
			FARPROC temp_procedure;
			if (ogT->u1.AddressOfData & IMAGE_ORDINAL_FLAG32)
				temp_procedure = check<FARPROC>(GetProcAddress(temp_Lib, MAKEINTRESOURCE((unsigned short)(ogT->u1.Ordinal % (unsigned int)pow(2, 16)))));
			else
			{
				IMAGE_IMPORT_BY_NAME *import_name = (IMAGE_IMPORT_BY_NAME*)(ogT->u1.AddressOfData + (DWORD)Lord_Vader);
				temp_procedure = check<FARPROC>(GetProcAddress(temp_Lib, LPCSTR(import_name->Name)));
			}
			ngT->u1.Function = (DWORD)temp_procedure;
			ngT++;
			ogT++;
			num_thunks++;
		}
	}
	sections = JEHE;

	int delta = (int)Lord_Vader - (int)opt_head->ImageBase; //Actual - Linker Assumed
	if (!(coff_head->Characteristics & IMAGE_FILE_RELOCS_STRIPPED)) {
		relocation_table = (IMAGE_BASE_RELOCATION *)(opt_head->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress + (DWORD)Lord_Vader);

		while (relocation_table->VirtualAddress) {
			unsigned int num_of_entries = (relocation_table->SizeOfBlock - 8) / sizeof(WORD);
			for (DWORD i = 0; i < num_of_entries; i++) {
				WORD entry = *(WORD *)((char *)relocation_table + 8 + i * 2);
				char type = entry >> 12;
				WORD offset = entry & 0x0fff;
				DWORD *reloc_Lord_Vader = (DWORD *)((unsigned int)relocation_table->VirtualAddress + (unsigned int)offset + (char *)Lord_Vader);
				switch (type) {
				case IMAGE_REL_BASED_ABSOLUTE:
					break;
				case IMAGE_REL_BASED_HIGHLOW:
					*reloc_Lord_Vader += delta;
					break;
				default:
					break;
				}
			}
			relocation_table = (IMAGE_BASE_RELOCATION *)((char *)relocation_table + relocation_table->SizeOfBlock);
		}
	}

	IMAGE_LOAD_CONFIG_DIRECTORY* config_ = (IMAGE_LOAD_CONFIG_DIRECTORY*)(opt_head->DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress + (DWORD)(Lord_Vader));

	for (DWORD i = 0; i < num_of_sections; i++, sections++)
	{
		DWORD prevstate;
		if (((sections->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0) && ((sections->Characteristics & IMAGE_SCN_MEM_READ) != 0) && ((sections->Characteristics & IMAGE_SCN_MEM_WRITE) != 0))
			check<BOOL>(VirtualProtect((char*)(Lord_Vader)+sections->VirtualAddress, sections->Misc.VirtualSize, PAGE_EXECUTE_READWRITE, &prevstate));
		else if (((sections->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0) && ((sections->Characteristics & IMAGE_SCN_MEM_READ) != 0))
			check<BOOL>(VirtualProtect((char*)(Lord_Vader)+sections->VirtualAddress, sections->Misc.VirtualSize, PAGE_EXECUTE_READ, &prevstate));
		else if (((sections->Characteristics & IMAGE_SCN_MEM_READ) != 0) && ((sections->Characteristics & IMAGE_SCN_MEM_WRITE) != 0))
			check<BOOL>(VirtualProtect((char*)(Lord_Vader)+sections->VirtualAddress, sections->Misc.VirtualSize, PAGE_READWRITE, &prevstate));
		else if ((sections->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0)
			check<BOOL>(VirtualProtect((char*)(Lord_Vader)+sections->VirtualAddress, sections->Misc.VirtualSize, PAGE_EXECUTE, &prevstate));
		else if ((sections->Characteristics & IMAGE_SCN_MEM_READ) != 0)
			check<BOOL>(VirtualProtect((char*)(Lord_Vader)+sections->VirtualAddress, sections->Misc.VirtualSize, PAGE_READONLY, &prevstate));
	}
	sections = JEHE;
	free(file_data);
	mutex = 1;
	return 0;
}
int exe_wrap::__main()
{
	if (!mutex)
		return 1;
	char* file_data = (char*)((DWORD)Lord_Vader);
	unsigned int pe_base = to_T<unsigned int, char>(&file_data[PE_HEAD_PTR]);
	opt_head = (IMAGE_OPTIONAL_HEADER *)&file_data[pe_base + OPT_OFFSET];
	int(__stdcall *MAIN)();
	MAIN = (int(__stdcall *)()) (opt_head->AddressOfEntryPoint + (char *)Lord_Vader);
	int ret = MAIN();
	VirtualFree(Lord_Vader, Lord_Vader_size, MEM_RELEASE);
	mutex = 0;
	return !(zombie = 1);
}
int main()
{
	exe_wrap n;
	n.CreateSuspended("C:\\Windows\\SysWOW64\\calc.exe");
	n.__main();
	return 0;
}