#include "morph_filter.h"

#include <obs-module.h>
#include <util/platform.h>
#include <util/dstr.h>

#define TAG "[Deepface-filter] "

class MorphFilter
{
public:
  MorphFilter(obs_source_t *source, obs_data_t *settings)
      : source_(source), eye_spacing_(0.0f)
  {
    blog(LOG_INFO, TAG "create");
    update(settings);
  }

  ~MorphFilter()
  {
    blog(LOG_INFO, TAG "destroy");
  }

  void update(obs_data_t *settings)
  {
    blog(LOG_INFO, TAG "update");
    eye_spacing_ = static_cast<float>(obs_data_get_double(settings, "eye_spacing"));
  }

  obs_properties_t *get_properties() const
  {
    blog(LOG_INFO, TAG "properties");
    obs_properties_t *props = obs_properties_create();
    obs_properties_add_float_slider(props, "eye_spacing", "Eye Spacing", -50.0, 50.0, 1.0);
    return props;
  }

  static void set_defaults(obs_data_t *settings)
  {
    blog(LOG_INFO, TAG "defaults");
    obs_data_set_default_double(settings, "eye_spacing", 0.0);
  }

  void render(gs_effect_t *effect)
  {
    // blog(LOG_INFO, TAG "render");
    if (!source_)
      return;

    obs_source_t *target = obs_filter_get_target(source_);
    if (target)
      obs_source_video_render(target);
  }

private:
  obs_source_t *source_;
  float eye_spacing_;
};

extern "C"
{

  static const char *morph_filter_get_name(void *)
  {
    blog(LOG_INFO, TAG "get_name");
    return "Deepface-filter";
  }

  static void *morph_filter_create(obs_data_t *settings, obs_source_t *source)
  {
    return static_cast<void *>(new MorphFilter(source, settings));
  }

  static void morph_filter_destroy(void *data)
  {
    delete static_cast<MorphFilter *>(data);
  }

  static void morph_filter_update(void *data, obs_data_t *settings)
  {
    static_cast<MorphFilter *>(data)->update(settings);
  }

  static obs_properties_t *morph_filter_properties(void *data)
  {
    return static_cast<MorphFilter *>(data)->get_properties();
  }

  static void morph_filter_defaults(obs_data_t *settings)
  {
    MorphFilter::set_defaults(settings);
  }

  static void morph_filter_render(void *data, gs_effect_t *effect)
  {
    static_cast<MorphFilter *>(data)->render(effect);
  }

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

} // extern "C"
