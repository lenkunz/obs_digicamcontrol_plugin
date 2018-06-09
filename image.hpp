
#ifndef DCC_OBS_IMAGE_HPP
#define DCC_OBS_IMAGE_HPP true

#include <graphics/graphics.h>

#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif // __cplusplus


struct jpeg_texture_info {
	gs_texture_t *texture;
	uint32_t width;
	uint32_t height;
	size_t size;
	size_t linesize;
};

EXTERNC uint8_t *decode_image_simd(struct jpeg_texture_info *info, void *data, size_t size);

#endif // !DCC_OBS_IMAGE_HPP
