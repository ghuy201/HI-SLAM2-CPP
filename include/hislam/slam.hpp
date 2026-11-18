#pragma once

#include <hislam/config/config.hpp>
#include <memory>
#include <opencv2/opencv.hpp>

namespace hislam {

struct ImageFrame {
  cv::Mat image;
  double timestamp;
};

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
  void track(const ImageFrame &frame);
  void terminate();
  void saveTrajectory(const std::string &path);
  void saveMap(const std::string &path);

private:
  class Impl;
  std::unique_ptr<Impl> pImpl_;
};
}; // namespace hislam
