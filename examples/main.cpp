#include <hislam/config.hpp>
#include <hislam/slam.hpp>
#include <iostream>

int main() {
  using Slam = hislam::Slam;
  using Config = hislam::Config;
  Config cfg{};
  Slam slam{cfg};
  slam.track();
}
