#include <hislam/logging/logger.hpp>
#include <hislam/slam.hpp>
#include <hislam/tracking/backend.hpp>
#include <hislam/tracking/frontend.hpp>
#include <hislam/tracking/motion_filter.hpp>
#include <vector>

#include <iostream>
#include <torch/torch.h>

namespace hislam {

class Slam::Impl {
public:
  explicit Impl(const HiConfig &config)
      : frontend_(config), backend_(config), motion_filter_(config),
        logger_("HI-SLAM2") {
    logger_.info("Starting HI-SLAM2");
  };
  void track(const tracking::ImageFrame &frame) {
    std::optional<torch::Tensor> intrinsics;
    bool is_last = false;

    images_.push_back(frame);
    motion_filter_.track(frame, intrinsics, is_last);
    frontend_.process(is_last);
  };

private:
  using ImageVector = std::vector<tracking::ImageFrame>;
  tracking::Frontend frontend_;
  tracking::Backend backend_;
  tracking::MotionFilter motion_filter_;
  logging::Logger logger_;
  ImageVector images_;
};

Slam::Slam(const HiConfig &config) : pImpl_(std::make_unique<Impl>(config)) {};
Slam::~Slam() {

};

void Slam::track(const tracking::ImageFrame &frame) { pImpl_->track(frame); }
} // namespace hislam
