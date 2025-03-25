#include "morph_filter.h"

struct morph_filter_data
{
  obs_source_t *source;
  float jaw_width;
};

const char *morph_filter_get_name(void *unused)
{
  return "Deepface-filter";
}

void *morph_filter_create(obs_data_t *settings, obs_source_t *source)
{
  auto *filter = new morph_filter_data;
  filter->source = source;
  filter->jaw_width = (float)obs_data_get_double(settings, "jaw_width");
  return filter;
}

void morph_filter_destroy(void *data)
{
  delete static_cast<morph_filter_data *>(data);
}

void morph_filter_update(void *data, obs_data_t *settings)
{
  auto *filter = static_cast<morph_filter_data *>(data);
  filter->jaw_width = (float)obs_data_get_double(settings, "jaw_width");
}

obs_properties_t *morph_filter_properties(void *data)
{
  obs_properties_t *props = obs_properties_create();
  obs_properties_add_float_slider(props, "jaw_width", "Jaw Width", -50.0, 50.0, 1.0);
  return props;
}

void morph_filter_defaults(obs_data_t *settings);

void morph_filter_render(void *data)
{
  auto *filter = static_cast<morph_filter_data *>(data);
  if (!filter || !filter->source)
    return;

  obs_source_video_render(filter->source);
}

obs_source_info morph_filter_info = {
    "face_morph_filter",    // id
    OBS_SOURCE_TYPE_FILTER, // type
    OBS_SOURCE_VIDEO,       // output_flags

    morph_filter_get_name, // get_name
    morph_filter_create,   // create
    morph_filter_destroy,  // destroy

    nullptr, // get_width
    nullptr, // get_height

    morph_filter_defaults,   // get_defaults
    morph_filter_properties, // get_properties
    morph_filter_update,     // update

    morph_filter_render // video_render
};

void morph_filter_defaults(obs_data_t *settings)
{
  obs_data_set_default_double(settings, "jaw_width", 0.0);
}
