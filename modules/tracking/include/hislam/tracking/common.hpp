#pragma once

#include <opencv2/opencv.hpp>

namespace hislam::tracking {
struct ImageFrame {
  cv::Mat image;
  double timestamp;
};

} // namespace hislam::tracking
