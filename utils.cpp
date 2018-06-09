#include <iostream>
#include <Windows.h>
#include <PathCch.h>
#include <curl/curl.h>

#include "utils.hpp"

using namespace std;

extern wstring get_directory()
{
	char path[300];
	HMODULE hm = NULL;

	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCSTR)&get_directory,
		&hm))
	{
		int ret = GetLastError();
		fprintf(stderr, "GetModuleHandle returned %d\n", ret);
	}
	GetModuleFileNameA(hm, path, sizeof(path));

	wchar_t wpath[300];
	mbstowcs(wpath, path, 300);
	PathCchRemoveFileSpec(wpath, 300);

	return wstring(wpath);
}

extern HMODULE load_module(wstring filename) {
	wstring path = get_directory() + L"\\" + filename;
	return LoadLibrary(path.c_str());
}

size_t _download_writer(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct download_file_data *info = (struct download_file_data *)userp;

	info->data = (char *)realloc(info->data, info->size + realsize + 1);

	if (info->data == NULL) {
		return 0;
	}

	memcpy(&(info->data[info->size]), contents, realsize);
	info->size += realsize;
	info->data[info->size] = 0;

	return realsize;
}

extern void download_file_data_destroy(struct download_file_data *info) {
	free(info->data);
	info->size = 0;
	info->success = false;
}

// https://curl.haxx.se/libcurl/c/getinmemory.html
extern struct download_file_data download_file(char *url)
{
	struct download_file_data info = {};
	info.success = true;
	info.size = 0;

	info.data = (char *)malloc(1);

	CURL *handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, _download_writer);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&info);
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	CURLcode res = curl_easy_perform(handle);

	if (res != CURLE_OK) {
		info.success = false;
		info.size = 0;

		free(info.data);
		info.data = NULL;
	}

	curl_easy_cleanup(handle);

	return info;
}
