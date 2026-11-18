#pragma once

#include <hislam/config/config.hpp>

namespace hislam::tracking {
class MotionFilter {
public:
  using HiConfig = hislam::config::Config;
  explicit MotionFilter(const HiConfig &config);
  void track()
};
}; // namespace hislam::tracking
