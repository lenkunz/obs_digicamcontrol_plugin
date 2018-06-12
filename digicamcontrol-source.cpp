#include <obs-module.h>
#include <graphics/graphics.h>
#include <curl/curl.h>
#include <iostream>
#include <atlstr.h>
#include <stdlib.h>

using namespace std;

#include "image.hpp"
#include "device-jpeg.hpp"

#ifdef USE_STANDALONELIB
#include "device-camera.hpp"
#endif // USE_STANDALONELIB

#include "digicamcontrol-source.hpp"
#include "utils.hpp"

extern struct obs_source_info digicamcontrol_source_info = {};

static void digicamcontrol_source_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);

	struct digicamcontrol_source *context = (struct digicamcontrol_source *)data;

	if (STANDALONELIB_CONTEXT(context)) {
#ifdef USE_STANDALONELIB
		get_texture_from_camera(context);
#endif
	}
	else {
		auto info = download_file((char *)context->url);

		if (info.success) {
			get_texture_from_jpeg(context, info.data, info.size);
		}
	}

	context->width = context->image.width;
	context->height = context->image.height;

	obs_source_draw(context->image.texture, 0, 0, context->image.width, context->image.height, false);
}

static const char *digicamcontrol_source_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("digicamcontrol Source");
}

static void digicamcontrol_source_update(void *data, obs_data_t *settings)
{
	struct digicamcontrol_source *context = (struct digicamcontrol_source *)data;

#ifdef USE_STANDALONELIB
	camera_function_t *camera = &context->camera;
#endif

	context->url = obs_data_get_string(settings, "webservice_url");

#ifdef USE_STANDALONELIB
	bool use_standalone = obs_data_get_bool(settings, "use_standalone_camera");

	if (use_standalone)
	{
		ensure_camera_module(context);

		int deviceIndex = (int)obs_data_get_int(settings, "device_index");
		const char *connection_string = obs_data_get_string(settings, "device_connection_string");

		wchar_t *w_connection_string = (wchar_t *)malloc(400);
		mbstowcs(w_connection_string, connection_string, 400);

		camera->device_select(deviceIndex, (LPWSTR *)w_connection_string);
		free(w_connection_string);

		const char *standbyimage_path = obs_data_get_string(settings, "image_standby_path");
		bool standbyimage_checked = obs_data_get_bool(settings, "image_standby_enable");

		wchar_t *w_standbyimage_path = (wchar_t*)malloc(400);
		mbstowcs(w_standbyimage_path, standbyimage_path, 400);

		camera->standbyimage_enable(standbyimage_checked);
		camera->standbyimage_set_path((LPWSTR*)w_standbyimage_path);
		free(w_standbyimage_path);
	}

	context->use_standalone = use_standalone;
#endif
}

static void *digicamcontrol_source_create(obs_data_t *settings, obs_source_t* source)
{
	struct digicamcontrol_source *context = (struct digicamcontrol_source *) bzalloc(sizeof(struct digicamcontrol_source));
	context->source = source;
	context->width = 640;
	context->height = 480;

	digicamcontrol_source_update((void*)context, settings);

	return context;
}

static void digicamcontrol_source_destroy(void *data)
{
	struct digicamcontrol_source *context = (struct digicamcontrol_source *)data;

#ifdef USE_STANDALONELIB
	destroy_camera_module(context);
#endif // USE_STANDALONELIB
	bfree(context);
}

static uint32_t digicamcontrol_source_width(void *data)
{
	struct digicamcontrol_source *context = (struct digicamcontrol_source *)data;

	return context->width;
}

static uint32_t digicamcontrol_source_height(void *data)
{
	struct digicamcontrol_source *context = (struct digicamcontrol_source *)data;

	return context->height;
}

#ifdef USE_STANDALONELIB
static const char *image_filter =
"All formats (*.bmp *.tga *.png *.jpeg *.jpg *.gif);;"
"BMP Files (*.bmp);;"
"Targa Files (*.tga);;"
"PNG Files (*.png);;"
"JPEG Files (*.jpeg *.jpg);;"
"GIF Files (*.gif)";
#endif

static obs_properties_t *digicamcontrol_source_properties(void *data)
{
	struct digicamcontrol_source *context = (struct digicamcontrol_source *)data;

	obs_properties_t *prop = obs_properties_create();

#ifdef USE_STANDALONELIB
	obs_properties_add_bool(prop, "use_standalone_camera", obs_module_text("Use standalone library (experimental)\nReload this window to see more options."));
#endif

	if (!STANDALONELIB_CONTEXT(context)) {
		obs_properties_add_text(prop, "webservice_url", obs_module_text("Web service"), OBS_TEXT_DEFAULT);
	}

#ifdef USE_STANDALONELIB
	int device_list_size = context->use_standalone ? context->camera.device_count() : 0;
	if (device_list_size > 0) {
		auto list = obs_properties_add_list(prop,
			"device_index",
			obs_module_text("Device"),
			OBS_COMBO_TYPE_LIST,
			OBS_COMBO_FORMAT_INT);
		obs_properties_add_text(prop, "device_connection_string", "Connection string", OBS_TEXT_DEFAULT);

		// Gathering string list
		int array_entry_size = 200;
		LPWSTR *list_p = (LPWSTR*)malloc(array_entry_size * sizeof(LPWSTR) * device_list_size);
		LPWSTR *current_entry = list_p;

		context->camera.device_get_list(list_p, array_entry_size * sizeof(LPWSTR), device_list_size);

		for (int i = 0; i < device_list_size; i++) {
			char *buffer = new char[array_entry_size];
			wcstombs(buffer, (wchar_t *)current_entry, array_entry_size);

			obs_property_list_add_int(list, buffer, i);
			current_entry += array_entry_size;
		}

		free(list_p);
	}

	wstring w_using_path = get_directory() + L"\\digicamstudio-libs\\liveview.jpg";
	char using_path[300];

	wcstombs(using_path, w_using_path.c_str(), 300);

	if (context->use_standalone) {
		obs_properties_add_bool(prop, "image_standby_enable", obs_module_text("Display image when not connected (standby, only standalone)"));
		obs_properties_add_path(prop,
			"image_standby_path",
			obs_module_text("Standby image"),
			OBS_PATH_FILE,
			image_filter,
			using_path);
	}
#endif
	return prop;
}

void digicamcontrol_source_default(obs_data_t *setting)
{
	obs_data_set_default_string(setting, "webservice_url", "http://127.0.0.1:5513/liveview.jpg");

#ifdef USE_STANDALONELIB
	obs_data_set_default_bool(setting, "use_standalone_camera", false);
	obs_data_set_default_string(setting, "device_connection_string", "<Auto>");

	wstring w_using_path = get_directory() + L"\\digicamstudio-libs\\liveview.jpg";
	char using_path[300];
	wcstombs(using_path, w_using_path.c_str(), 300);

	obs_data_set_default_bool(setting, "image_standby_enable", true);
	obs_data_set_default_string(setting, "image_standby_path", using_path);
#endif
}

void init_source()
{
	struct obs_source_info *i = &digicamcontrol_source_info;

	i->id = "digicamcontrol source";
	i->type = OBS_SOURCE_TYPE_INPUT;
	i->output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_DO_NOT_DUPLICATE;
	i->get_name = digicamcontrol_source_name;
	i->create = digicamcontrol_source_create;
	i->destroy = digicamcontrol_source_destroy;
	i->update = digicamcontrol_source_update;
	i->video_render = digicamcontrol_source_render;
	i->get_width = digicamcontrol_source_width;
	i->get_height = digicamcontrol_source_height;
	i->get_properties = digicamcontrol_source_properties;
	i->get_defaults = digicamcontrol_source_default;

	curl_global_init(CURL_GLOBAL_ALL);
}
