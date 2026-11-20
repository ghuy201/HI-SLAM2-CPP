#pragma once

#include <hislam/config/config.hpp>
#include <hislam/tracking/common.hpp>
#include <memory>

namespace hislam {

class Slam {
public:
  using HiConfig = config::Config;

  explicit Slam(const HiConfig &config);
  ~Slam();

  Slam(const Slam &) = delete;
  Slam &operator=(const Slam &) = delete;

  Slam(const Slam &&) = delete;
  Slam &operator=(const Slam &&) = delete;

  void start();
  void track(const tracking::ImageFrame &frame);
  void terminate();
  void saveTrajectory(const std::string &path);
  void saveMap(const std::string &path);

private:
  class Impl;
  std::unique_ptr<Impl> pImpl_;
};
}; // namespace hislam
