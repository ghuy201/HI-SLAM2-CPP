#include <hislam/slam.hpp>
#include <hislam/tracking/backend.hpp>
#include <hislam/tracking/frontend.hpp>
#include <vector>

#include <iostream>
#include <torch/torch.h>

namespace hislam {

class Slam::Impl {
public:
  explicit Impl(const HiConfig &config)
      : frontend_(config), backend_(config) {};
  void track(const ImageFrame &frame) { images_.push_back(frame); };

private:
  using ImageVector = std::vector<ImageFrame>;
  tracking::Frontend frontend_;
  tracking::Backend backend_;
  ImageVector images_;
};

Slam::Slam(const HiConfig &config) : pImpl_(std::make_unique<Impl>(config)) {
  std::cout << "Loading Hi-SLAM2" << "\n";
};
Slam::~Slam() {

};

void Slam::track(const ImageFrame &frame) { pImpl_->track(frame); }
} // namespace hislam
