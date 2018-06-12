#pragma once
#include <iostream>
#include <Windows.h>

struct download_file_data {
	size_t	size;
	char	*data;
	bool	success;
};

struct download_file_data download_file(char *url);
void download_file_data_destroy(struct download_file_data *info);

std::wstring get_directory();
HMODULE load_module(std::wstring filename);
void unload_module(HMODULE ext);
