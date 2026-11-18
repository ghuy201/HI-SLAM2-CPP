#pragma once

#include <iostream>

#include <hislam/config/config.hpp>
namespace hislam::tracking {
class Frontend {
public:
  using HiConfig = config::Config;
  Frontend(const HiConfig &config);
  void sayHello() { std::cout << "Hi from hi-slam" << "\n"; }

private:
};
} // namespace hislam::tracking
