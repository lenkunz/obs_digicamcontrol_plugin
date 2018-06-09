#ifdef USE_STANDALONELIB

#include <iostream>
#include <Windows.h>

#include "utils.hpp"
#include "image.hpp"
#include "device-camera.hpp"
#include "digicamcontrol-source.hpp"

extern void ensure_camera_module(struct digicamcontrol_source *context)
{
	camera_function_t *c = &(context->camera);
	if (c->ext_module == NULL) {
		c->ext_module = load_module(L"\\digicamstudio-libs\\DigicamControlInterface.dll");

		c->image_bgra = (c_image_get_bgra)GetProcAddress(c->ext_module, "image_get_bgra");
		c->image_width = (c_image_get_width)GetProcAddress(c->ext_module, "image_get_width");
		c->image_height = (c_image_get_height)GetProcAddress(c->ext_module, "image_get_height");
		c->image_line_size = (c_image_get_line_size)GetProcAddress(c->ext_module, "image_get_line_size");
		c->image_size = (c_image_get_size)GetProcAddress(c->ext_module, "image_get_size");

		c->device_count = (c_device_count)GetProcAddress(c->ext_module, "device_count");
		c->device_get_list = (c_device_get_list)GetProcAddress(c->ext_module, "device_get_list");
		c->device_select = (c_device_select)GetProcAddress(c->ext_module, "device_select");

		c->standbyimage_set_path = (c_standbyimage_set_path)GetProcAddress(c->ext_module, "standbyimage_set_path");
		c->standbyimage_get_path = (c_standbyimage_get_path)GetProcAddress(c->ext_module, "standbyimage_get_path");
		c->standbyimage_enable = (c_standbyimage_enable)GetProcAddress(c->ext_module, "standbyimage_enable");

		c->initialize = (c_initialize)GetProcAddress(c->ext_module, "initialize");
	}

	c->initialize();
}

extern void get_texture_from_camera(struct digicamcontrol_source *context)
{
	camera_function_t *camera = &context->camera;
	struct jpeg_texture_info *info = &context->image;

	if (camera->image_size() <= 0) {
		return;
	}

	info->width = camera->image_width();
	info->height = camera->image_height();
	info->linesize = camera->image_line_size();
	info->size = camera->image_size();

	void *image = malloc(info->size);
	camera->image_bgra(image);

	gs_texture_t *new_texture = gs_texture_create(info->width, info->height, GS_BGRA, 1, NULL, GS_DYNAMIC);
	gs_texture_set_image(new_texture, (uint8_t *)image, info->linesize, false);

	free(image);

	gs_texture_destroy(info->texture);
	info->texture = new_texture;
}

#endif
