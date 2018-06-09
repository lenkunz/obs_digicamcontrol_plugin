#include <graphics/graphics.h>
#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include <PathCch.h>

#include "device-jpeg.hpp"
#include "utils.hpp"
#include "image.hpp"
#include "device-jpeg.hpp"
#include "digicamcontrol-source.hpp"

using namespace std;

extern void get_texture_from_jpeg(struct digicamcontrol_source *context, void *data, size_t size)
{
	struct jpeg_texture_info *info = &context->image;
	uint8_t *image = decode_image_simd(info, data, size);

	gs_texture_t *new_texture = gs_texture_create(info->width, info->height, GS_BGRA, 1, NULL, GS_DYNAMIC);
	gs_texture_set_image(new_texture, image, info->linesize, false);

	free(image);

	gs_texture_destroy(info->texture);
	info->texture = new_texture;
}
