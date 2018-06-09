#ifndef DCC_OBS_DEVICE_JPEG_HPP
#define DCC_OBS_DEVICE_JPEG_HPP true

#include <graphics/graphics.h>
#include "utils.hpp"

void jpeg_texture_info_destroy(struct jpeg_texture_info *info);
void get_texture_from_jpeg(struct digicamcontrol_source *context, void *data, size_t size);

#endif // !DCC_OBS_DEVICE_JPEG_HPP
