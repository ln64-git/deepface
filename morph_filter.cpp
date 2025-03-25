#include "morph_filter.h"

struct morph_filter_data
{
  obs_source_t *source;
  float jaw_width;
};

static const char *morph_filter_get_name(void *unused)
{
  return "Face Morph Filter";
}

static void *morph_filter_create(obs_data_t *settings, obs_source_t *source)
{
  auto *filter = new morph_filter_data;
  filter->source = source;
  filter->jaw_width = (float)obs_data_get_double(settings, "jaw_width");
  return filter;
}

static void morph_filter_destroy(void *data)
{
  delete static_cast<morph_filter_data *>(data);
}

static obs_properties_t *morph_filter_properties(void *data)
{
  obs_properties_t *props = obs_properties_create();
  obs_properties_add_float_slider(props, "jaw_width", "Jaw Width", -50.0, 50.0, 1.0);
  return props;
}

static void morph_filter_update(void *data, obs_data_t *settings)
{
  auto *filter = static_cast<morph_filter_data *>(data);
  filter->jaw_width = (float)obs_data_get_double(settings, "jaw_width");
}

static void morph_filter_render(void *data, gs_effect_t *effect)
{
  auto *filter = static_cast<morph_filter_data *>(data);
  obs_source_video_render(filter->source);
}

struct obs_source_info morph_filter_info = {
    .id = "face_morph_filter",
    .type = OBS_SOURCE_TYPE_FILTER,
    .output_flags = OBS_SOURCE_VIDEO,
    .get_name = morph_filter_get_name,
    .create = morph_filter_create,
    .destroy = morph_filter_destroy,
    .update = morph_filter_update,
    .get_properties = morph_filter_properties,
    .video_render = morph_filter_render,
};
