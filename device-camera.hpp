#if defined USE_STANDALONELIB && !defined DCC_OBS_DEVICE_CAMERA_HPP
#define DCC_OBS_DEVICE_CAMERA_HPP true

#include <graphics/graphics.h>
#include <Windows.h>

typedef void(__cdecl *c_image_get_bgra)(void* data_buffer);
typedef uint32_t(__cdecl *c_image_get_width)();
typedef uint32_t(__cdecl *c_image_get_height)();
typedef uint32_t(__cdecl *c_image_get_line_size)();
typedef uint32_t(__cdecl *c_image_get_size)();
typedef void(__cdecl *c_initialize)();

typedef uint32_t(__cdecl *c_device_count)();
typedef void(__cdecl *c_device_get_list)(LPWSTR* data_array, int32_t member_size, int32_t array_length);
typedef void(__cdecl *c_device_select)(int, LPWSTR* data_array);

typedef void(__cdecl *c_standbyimage_set_path)(LPWSTR* path);
typedef void(__cdecl *c_standbyimage_get_path)(LPWSTR* result, int32_t max_size);
typedef void(__cdecl *c_standbyimage_enable)(int);

typedef struct camera_function {
	HMODULE ext_module = NULL;
	c_image_get_bgra image_bgra;
	c_image_get_width image_width;
	c_image_get_height image_height;
	c_image_get_size image_size;
	c_image_get_line_size image_line_size;

	c_device_count device_count;
	c_device_get_list device_get_list;
	c_device_select device_select;

	c_standbyimage_set_path standbyimage_set_path;
	c_standbyimage_get_path standbyimage_get_path;
	c_standbyimage_enable standbyimage_enable;

	c_initialize initialize;
} camera_function_t;

extern void ensure_camera_module(struct digicamcontrol_source *context);
extern void get_texture_from_camera(struct digicamcontrol_source *context);

#endif // !DCC_OBS_DEVICE_CAMERA_HPP
