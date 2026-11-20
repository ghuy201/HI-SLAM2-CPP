#include <cmath>
#include <hislam/tracking/motion_filter.hpp>
#include <opencv2/opencv.hpp>

namespace hislam::tracking {

MotionFilter::MotionFilter(const HiConfig config, torch::Device device)
    : config_(std::move(config.tracking.motion_filter)), device_(device) {

  const auto options =
      torch::TensorOptions().dtype(torch::kFloat32).device(device_);
  mean_ =
      torch::tensor({0.485f, 0.456f, 0.406f}, options).view({1, 1, 3, 1, 1});
  std_ = torch::tensor({0.229, 0.224, 0.225}, options).view({1, 1, 3, 1, 1});
};

double MotionFilter::computeSharpness(const torch::Tensor &image) {
  auto cpu = image.detach()
                 .to(torch::kCPU)
                 .to(torch::kUInt8)
                 .permute({1, 2, 0})
                 .contiguous();
  const auto height = static_cast<int>(cpu.size(0));
  const auto width = static_cast<int>(cpu.size(1));

  cv::Mat rgb(height, width, CV_8UC3, cpu.data_ptr<std::uint8_t>());
  cv::Mat gray;
  cv::cvtColor(rgb, gray, cv::COLOR_RGB2GRAY);
  cv::Mat laplacian;
  cv::Laplacian(gray, laplacian, CV_64F);
  cv::Scalar mean;
  cv::Scalar stddev;
  cv::meanStdDev(laplacian, mean, stddev);
  return stddev[0] * stddev[0];
};

torch::Tensor MotionFilter::normaliseImage(const torch::Tensor &image) {
  auto inputs = image.unsqueeze(0).to(device_, torch::kFloat32).div(255.0f);
  return (inputs - mean_) / std_;
};

void MotionFilter::track(const ImageFrame &frame,
                         std::optional<torch::Tensor> intrinsics,
                         bool is_last) {
  torch::NoGradGuard no_grad;
  torch::Tensor img_tensor =
      torch::from_blob(
          frame.image.data,
          {frame.image.rows, frame.image.cols, frame.image.channels()},
          torch::kUInt8)
          .clone();
  cv::Size img_size = frame.image.size();
  int height = std::floor(img_size.height / 8);
  int width = std::floor(img_size.width / 8);

  const double sharpness = computeSharpness(img_tensor);
  torch::Tensor inputs = normaliseImage(img_tensor);
};
}; // namespace hislam::tracking
