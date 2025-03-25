#include "morph_filter.h"

#include <obs-module.h>
#include <util/platform.h>
#include <util/dstr.h>

#define TAG "[Deepface-filter] "

static inline void obs_source_process_filter(obs_source_t *filter, gs_effect_t *effect, int width, int height)
{
  if (!filter)
    return;

  obs_source_t *target = obs_filter_get_target(filter);
  if (!target)
    return;

  obs_source_video_render(target);
}

struct morph_filter_data
{
  obs_source_t *source;
  float eye_spacing;
};

const char *morph_filter_get_name(void *unused)
{
  blog(LOG_INFO, TAG "get_name");
  return "Deepface-filter";
}

void *morph_filter_create(obs_data_t *settings, obs_source_t *source)
{
  blog(LOG_INFO, TAG "create");
  auto *filter = new morph_filter_data;
  filter->source = source;
  filter->eye_spacing = (float)obs_data_get_double(settings, "eye_spacing");
  return filter;
}

void morph_filter_destroy(void *data)
{
  blog(LOG_INFO, TAG "destroy");
  delete static_cast<morph_filter_data *>(data);
}

void morph_filter_update(void *data, obs_data_t *settings)
{
  blog(LOG_INFO, TAG "update");
  auto *filter = static_cast<morph_filter_data *>(data);
  if (!filter)
    return;
  filter->eye_spacing = (float)obs_data_get_double(settings, "eye_spacing");
}

obs_properties_t *morph_filter_properties(void *data)
{
  blog(LOG_INFO, TAG "properties");
  obs_properties_t *props = obs_properties_create();
  obs_properties_add_float_slider(props, "eye_spacing", "Eye Spacing", -50.0, 50.0, 1.0);
  return props;
}

void morph_filter_defaults(obs_data_t *settings)
{
  blog(LOG_INFO, TAG "defaults");
  obs_data_set_default_double(settings, "eye_spacing", 0.0);
}

void morph_filter_render(void *data, gs_effect_t *effect)
{
  auto *filter = static_cast<morph_filter_data *>(data);
  if (!filter || !filter->source)
    return;

  obs_source_process_filter(filter->source, effect, 0, 0);
}

obs_source_info morph_filter_info = {
    "face_morph_filter",
    OBS_SOURCE_TYPE_FILTER,
    OBS_SOURCE_VIDEO,

    morph_filter_get_name,
    morph_filter_create,
    morph_filter_destroy,

    nullptr,
    nullptr,

    morph_filter_defaults,
    morph_filter_properties,
    morph_filter_update,
    nullptr, // activate
    nullptr, // deactivate
    nullptr, // show
    nullptr, // hide
    nullptr, // video_tick
    morph_filter_render};
