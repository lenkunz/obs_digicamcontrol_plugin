#include <graphics/graphics.h>
#include <turbojpeg.h>
#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include <PathCch.h>
#include <exception>

#include "digicamcontrol-image.hpp"

using namespace std;

wstring get_directory()
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

HMODULE load_module(wstring filename) {
	wstring path = get_directory() + L"\\" + filename;
	return LoadLibrary(path.c_str());
}

extern "C" void jpeg_texture_info_destroy(struct jpeg_texture_info *info) {
	if (info->texture != NULL) {
		gs_texture_destroy(info->texture);
	}

	info->width = 0;
	info->height = 0;
	info->linesize = 0;
	info->size = 0;
}

uint8_t *decode_image_simd(struct jpeg_texture_info *info, void *data, size_t size)
{
	int width = 0, height = 0;
	tjhandle handle = tjInitDecompress();
	tjDecompressHeader(handle, (unsigned char*)data, size, &width, &height);

	uint8_t *image = (uint8_t *) malloc(width * height * 4);

	tjDecompress2(handle, (unsigned char*)data, size, image, width, 0, height, TJPF_BGRA, TJFLAG_FASTDCT);
	tjDestroy(handle);

	info->width = width;
	info->height = height;
	info->linesize = width * 4;
	info->size = width * height * 4;
	return image;
}

typedef void(__cdecl *c_get_image_bgra)(void*);
typedef uint32_t(__cdecl *c_get_image_width)();
typedef uint32_t(__cdecl *c_get_image_height)();
typedef uint32_t(__cdecl *c_get_image_size)();
typedef void(__cdecl *c_initialize)();
typedef char*(__cdecl *c_get_string)();

HMODULE extModule;
c_get_image_bgra camera_image_bgra;
c_get_image_width camera_image_width;
c_get_image_height camera_image_height;
c_get_image_size camera_image_size;
c_initialize camera_initialize;
c_get_string camera_test_string;

extern "C" void init_camera_module() {
	extModule = load_module(L"DigicamControlInterface.dll");
	camera_image_bgra = (c_get_image_bgra)GetProcAddress(extModule, "get_image_bgra");
	camera_image_width = (c_get_image_width)GetProcAddress(extModule, "get_image_width");
	camera_image_height = (c_get_image_height)GetProcAddress(extModule, "get_image_height");
	camera_image_size = (c_get_image_size)GetProcAddress(extModule, "get_image_size");
	camera_initialize = (c_initialize)GetProcAddress(extModule, "initialize");
	camera_test_string = (c_get_string)GetProcAddress(extModule, "get_string");
}

extern "C" void get_texture_from_camera(struct jpeg_texture_info *info)
{
	camera_initialize();

	if (camera_image_size() <= 0) {
		return;
	}

	info->width = camera_image_width();
	info->height = camera_image_height();
	info->linesize = info->width * 4;
	info->size = camera_image_size();

	void *image = malloc(info->size);
	camera_image_bgra(image);

	gs_texture_t *new_texture = gs_texture_create(info->width, info->height, GS_BGRA, 1, NULL, GS_DYNAMIC);
	gs_texture_set_image(new_texture, (uint8_t *)image, info->linesize, false);

	free(image);

	gs_texture_destroy(info->texture);
	info->texture = new_texture;
}

extern "C" void get_texture_from_jpeg(struct jpeg_texture_info *info, void *data, size_t size)
{
	uint8_t *image = decode_image_simd(info, data, size);

	gs_texture_t *new_texture = gs_texture_create(info->width, info->height, GS_BGRA, 1, NULL, GS_DYNAMIC);
	gs_texture_set_image(new_texture, image, info->linesize, false);

	free(image);

	gs_texture_destroy(info->texture);
	info->texture = new_texture;
}
