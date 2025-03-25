#include "morph_filter.h"

#include <obs-module.h>
#include <util/platform.h>
#include <util/dstr.h>

#define TAG "[Deepface-filter] "

// Helper: Render target source through the filter chain
static inline void obs_source_process_filter(obs_source_t *filter, gs_effect_t *effect, int width, int height)
{
  if (!filter)
    return;
  obs_source_t *target = obs_filter_get_target(filter);
  if (target)
    obs_source_video_render(target);
}

// Filter state
struct morph_filter_data
{
  obs_source_t *source;
  float eye_spacing;
};

// Name shown in OBS filter list
static const char *morph_filter_get_name(void *)
{
  blog(LOG_INFO, TAG "get_name");
  return "Deepface-filter";
}

// Create and initialize filter instance
static void *morph_filter_create(obs_data_t *settings, obs_source_t *source)
{
  blog(LOG_INFO, TAG "create");

  auto *filter = new morph_filter_data;
  filter->source = source;
  filter->eye_spacing = (float)obs_data_get_double(settings, "eye_spacing");

  return filter;
}

// Destroy filter instance
static void morph_filter_destroy(void *data)
{
  blog(LOG_INFO, TAG "destroy");
  delete static_cast<morph_filter_data *>(data);
}

// Update filter when settings change
static void morph_filter_update(void *data, obs_data_t *settings)
{
  blog(LOG_INFO, TAG "update");

  auto *filter = static_cast<morph_filter_data *>(data);
  if (filter)
    filter->eye_spacing = (float)obs_data_get_double(settings, "eye_spacing");
}

// Define filter properties shown in UI
static obs_properties_t *morph_filter_properties(void *)
{
  blog(LOG_INFO, TAG "properties");

  obs_properties_t *props = obs_properties_create();
  obs_properties_add_float_slider(props, "eye_spacing", "Eye Spacing", -50.0, 50.0, 1.0);
  return props;
}

// Define default settings
static void morph_filter_defaults(obs_data_t *settings)
{
  blog(LOG_INFO, TAG "defaults");
  obs_data_set_default_double(settings, "eye_spacing", 0.0);
}

// Render the filtered output
static void morph_filter_render(void *data, gs_effect_t *effect)
{
  blog(LOG_INFO, TAG "render");

  auto *filter = static_cast<morph_filter_data *>(data);
  if (filter && filter->source)
    obs_source_process_filter(filter->source, effect, 0, 0);
}

// Register filter
struct obs_source_info morph_filter_info = {
    .id = "face_morph_filter",
    .type = OBS_SOURCE_TYPE_FILTER,
    .output_flags = OBS_SOURCE_VIDEO,

    .get_name = morph_filter_get_name,
    .create = morph_filter_create,
    .destroy = morph_filter_destroy,

    .get_width = nullptr,
    .get_height = nullptr,

    .get_defaults = morph_filter_defaults,
    .get_properties = morph_filter_properties,
    .update = morph_filter_update,

    .activate = nullptr,
    .deactivate = nullptr,
    .show = nullptr,
    .hide = nullptr,
    .video_tick = nullptr,
    .video_render = morph_filter_render,
};
