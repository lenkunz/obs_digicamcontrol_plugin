#pragma once
#include <graphics/graphics.h>
#include "utils.hpp"

void jpeg_texture_info_destroy(struct jpeg_texture_info *info);
void get_texture_from_jpeg(struct digicamcontrol_source *context, void *data, size_t size);
