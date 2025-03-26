#include "morph_filter.h"
#include <opencv2/imgproc.hpp>

#include <obs-module.h>
#include <util/platform.h>
#include <util/dstr.h>

#define TAG "[Deepface-filter] "

#include "landmark_detector/landmark_detector.h" // 👈 Add this
extern "C"
{
#include <graphics/effect.h>   // 🔑 Required for gs_effect_get_technique_by_name
#include <graphics/graphics.h> // Also grants access to gs_draw_sprite etc.
}

class MorphFilter
{
public:
  MorphFilter(obs_source_t *source, obs_data_t *settings)
      : source_(source), eye_spacing_(0.0f),
        detector_("../../models/face_landmarks.onnx") // 👈 Adjust path as needed
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
    blog(LOG_INFO, TAG "properties"); // Is this being logged?
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
    if (!source_)
      return;

    uint32_t width = obs_source_get_width(source_);
    uint32_t height = obs_source_get_height(source_);
    if (width == 0 || height == 0)
      return;

    gs_texrender_t *texrender = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
    gs_texrender_reset(texrender);

    if (!gs_texrender_begin(texrender, width, height))
    {
      gs_texrender_destroy(texrender);
      return;
    }

    obs_source_video_render(obs_filter_get_target(source_));
    gs_texrender_end(texrender);

    gs_texture_t *tex = gs_texrender_get_texture(texrender);
    if (!tex)
    {
      gs_texrender_destroy(texrender);
      return;
    }

    uint8_t *data = nullptr;
    uint32_t linesize = 0;
    if (!gs_texture_map(tex, &data, &linesize))
    {
      gs_texrender_destroy(texrender);
      return;
    }

    cv::Mat rgba(height, width, CV_8UC4, data, linesize);
    cv::Mat bgr;
    cv::cvtColor(rgba, bgr, cv::COLOR_RGBA2BGR);
    gs_texture_unmap(tex);

    auto landmarks = detector_.detect(bgr);
    for (const auto &pt : landmarks)
    {
      cv::circle(bgr, pt, 2, cv::Scalar(0, 255, 0), -1);
    }

    cv::cvtColor(bgr, rgba, cv::COLOR_BGR2RGBA);
    gs_texture_set_image(tex, rgba.data, rgba.step.p[0], false);

    // 👇 This is the fix:
    gs_eparam_t *image = gs_effect_get_param_by_name(effect, "image");
    gs_effect_set_texture(image, tex);

    while (gs_effect_loop(effect, "Draw"))
    {
      gs_draw_sprite(tex, 0, width, height);
    }

    gs_texrender_destroy(texrender);
  }

private:
  obs_source_t *source_;
  float eye_spacing_;
  LandmarkDetector detector_; // 👈 ONNX landmark detector
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
