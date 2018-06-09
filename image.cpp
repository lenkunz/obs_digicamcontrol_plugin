#include <turbojpeg.h>
#include <stdlib.h>
#include <iostream>

#include "image.hpp"

void jpeg_texture_info_destroy(struct jpeg_texture_info *info) {
	if (info->texture != NULL) {
		gs_texture_destroy(info->texture);
	}

	info->width = 0;
	info->height = 0;
	info->linesize = 0;
	info->size = 0;
}

EXTERNC uint8_t *decode_image_simd(struct jpeg_texture_info *info, void *data, size_t size)
{
	int width = 0, height = 0;
	tjhandle handle = tjInitDecompress();
	tjDecompressHeader(handle, (unsigned char*)data, size, &width, &height);

	uint8_t *image = (uint8_t *)malloc(width * height * 4);

	tjDecompress2(handle, (unsigned char*)data, size, image, width, 0, height, TJPF_BGRA, TJFLAG_FASTDCT);
	tjDestroy(handle);

	info->width = width;
	info->height = height;
	info->linesize = width * 4;
	info->size = width * height * 4;
	return image;
}
