#include <obs-module.h>
#include "digicamcontrol-source.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("digicamcontrol-plugin", "en-US")

bool obs_module_load(void)
{
	init_source();

	obs_register_source(&digicamcontrol_source_info);
	return true;
}
