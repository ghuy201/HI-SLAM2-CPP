#include <filesystem>
#include <hislam/config/config.hpp>
#include <hislam/slam.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
  using Slam = hislam::Slam;
  using Config = hislam::config::Config;
  const auto config_path =
      argc > 1 ? std::filesystem::path(argv[1])
               : std::filesystem::path("../config/euroc_config.yaml");
  Config cfg{config_path};
  Slam slam{cfg};
  for (auto i{0uz}; i < 100; ++i) {
    cv::Mat new_img = cv::Mat::zeros(cv::Size(640, 480), CV_8UC1);
    slam.track(hislam::ImageFrame{new_img, 0.0});
  };
}
