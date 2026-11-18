#pragma once

#include <hislam/config/config.hpp>

namespace hislam::tracking {
class Backend {
public:
  using HiConfig = config::Config;
  Backend(const HiConfig &config);
};
}; // namespace hislam::tracking
