#pragma once

#include <iostream>

#include <hislam/config.hpp>
namespace hislam::tracking {
class Frontend {
public:
  Frontend(const Config &config);
  void sayHello() { std::cout << "Hi from hi-slam" << "\n"; }

private:
};
} // namespace hislam::tracking
