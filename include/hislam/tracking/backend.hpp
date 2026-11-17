#pragma once

#include <hislam/config.hpp>

namespace hislam::tracking {
class Backend {
public:
  Backend(const Config &config);
};
}; // namespace hislam::tracking
