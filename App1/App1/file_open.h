#pragma once

int dump_kern32(DWORD kernbase, bool local, bool remote);
void create_file_sample(Platform::String^ tar_filename,
	unsigned char* out_buf, size_t out_size, bool);