#include <hislam/slam.hpp>
#include <hislam/tracking/backend.hpp>
#include <hislam/tracking/frontend.hpp>

#include <iostream>

namespace hislam {

class Slam::Impl {
public:
  explicit Impl(const Config &config) : frontend_(config), backend_(config) {}
  void track() { std::cout << "Tracking\n"; };

private:
  tracking::Frontend frontend_;
  tracking::Backend backend_;
};

Slam::Slam(const Config &config) : pImpl_(std::make_unique<Impl>(config)) {
  std::cout << "Loading Hi-SLAM2" << "\n";
};
Slam::~Slam() {

};

void Slam::track() { pImpl_->track(); }
} // namespace hislam
