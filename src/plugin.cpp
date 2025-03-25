#include <obs-module.h>
#include "../morph_filter/morph_filter.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("deepface-filter", "en-US")

extern "C" bool obs_module_load(void)
{
    obs_register_source(&morph_filter_info);
    blog(LOG_INFO, "[Deepface-filter] Plugin loaded.");
    return true;
}
