extern "C"
{
#include <obs-module.h>
}

#include "landmark_detector.h"

#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>

#define TAG "[LandmarkDetector] "

LandmarkDetector::LandmarkDetector(const std::string &model_path)
    : env_(ORT_LOGGING_LEVEL_WARNING, "landmark"), session_(nullptr), memory_info_(nullptr)
{
  blog(LOG_INFO, TAG "Loading model: %s", model_path.c_str());

  Ort::SessionOptions session_options;
  session_options.SetIntraOpNumThreads(1);
  session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

  session_ = Ort::Session(env_, model_path.c_str(), session_options);
  memory_info_ = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

  Ort::TypeInfo input_info = session_.GetInputTypeInfo(0);
  auto input_tensor_info = input_info.GetTensorTypeAndShapeInfo();
  input_dims_ = input_tensor_info.GetShape();

  input_width_ = static_cast<int>(input_dims_[3]);
  input_height_ = static_cast<int>(input_dims_[2]);
}

std::vector<cv::Point2f> LandmarkDetector::detect(const cv::Mat &frame)
{
  blog(LOG_INFO, TAG "Detecting landmarks");

  cv::Mat resized, rgb, float_img;
  cv::resize(frame, resized, cv::Size(input_width_, input_height_));
  cv::cvtColor(resized, rgb, cv::COLOR_BGR2RGB);
  rgb.convertTo(float_img, CV_32F, 1.0 / 255.0);

  std::vector<float> input_tensor_values(float_img.total() * 3);
  std::memcpy(input_tensor_values.data(), float_img.data, input_tensor_values.size() * sizeof(float));

  std::vector<int64_t> input_shape = {1, 3, input_height_, input_width_};
  Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
      memory_info_, input_tensor_values.data(), input_tensor_values.size(),
      input_shape.data(), input_shape.size());

  auto output_tensors = session_.Run(Ort::RunOptions{nullptr},
                                     &input_node_name_, &input_tensor, 1,
                                     &output_node_name_, 1);

  float *output = output_tensors.front().GetTensorMutableData<float>();

  std::vector<cv::Point2f> landmarks;
  for (size_t i = 0; i < 68; ++i)
  {
    float x = output[i * 2] * frame.cols;
    float y = output[i * 2 + 1] * frame.rows;
    landmarks.emplace_back(x, y);
  }

  return landmarks;
}
