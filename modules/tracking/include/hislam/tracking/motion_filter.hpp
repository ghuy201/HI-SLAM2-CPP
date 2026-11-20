#pragma once

#include <hislam/config/config.hpp>
#include <hislam/tracking/common.hpp>
#include <torch/torch.h>
#include <utility>

namespace hislam::tracking {
class MotionFilter {
public:
  using HiConfig = hislam::config::Config;
  using MotionFilterConfig = hislam::config::MotionFilterConfig;
  explicit MotionFilter(const HiConfig config,
                        torch::Device device = torch::kCUDA);
  void track(const ImageFrame &frame, std::optional<torch::Tensor> intrinsics,
             bool is_last = false);

private:
  [[nodiscard]] static double computeSharpness(const torch::Tensor &image);
  [[nodiscard]] torch::Tensor normaliseImage(const torch::Tensor &image);
  MotionFilterConfig config_;

  torch::Device device_;
  torch::Tensor mean_;
  torch::Tensor std_;
};
}; // namespace hislam::tracking
