#pragma once
#include <obs-module.h>
#include "image.hpp"

#ifdef USE_STANDALONELIB
#include "device-camera.hpp"
#endif

struct digicamcontrol_source {
	obs_source_t			*source;

	const char			*url;
#ifdef USE_STANDALONELIB
	bool				use_standalone;
#endif // USE_STANDALONELIB
	uint32_t			width;
	uint32_t			height;

	struct jpeg_texture_info	image;
#ifdef USE_STANDALONELIB
	camera_function_t		camera;
#endif // USE_STANDALONELIB
};

void init_source();
extern struct obs_source_info digicamcontrol_source_info;

#ifdef USE_STANDALONELIB
#define STANDALONELIB_CONTEXT(context)	context->use_standalone
#else
#define STANDALONELIB_CONTEXT(context)	false
#endif
