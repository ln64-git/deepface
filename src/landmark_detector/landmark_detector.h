#pragma once

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <onnxruntime_cxx_api.h>

class LandmarkDetector
{
public:
  explicit LandmarkDetector(const std::string &model_path);
  std::vector<cv::Point2f> detect(const cv::Mat &frame);

private:
  Ort::Env env_;
  Ort::Session session_;
  Ort::MemoryInfo memory_info_;

  std::vector<int64_t> input_dims_;
  int input_width_;
  int input_height_;

  const char *input_node_name_ = "input";   // ← Change to actual model input name
  const char *output_node_name_ = "output"; // ← Change to actual model output name
};
