#pragma once

#include <hislam/config.hpp>
#include <memory>

namespace hislam {
class Slam {
public:
  explicit Slam(const Config &config);
  ~Slam();

  Slam(const Slam &) = delete;
  Slam &operator=(const Slam &) = delete;

  Slam(const Slam &&) = delete;
  Slam &operator=(const Slam &&) = delete;

  void start();
  void track();
  void terminate();
  void saveTrajectory(const std::string &path);
  void saveMap(const std::string &path);

private:
  class Impl;
  std::unique_ptr<Impl> pImpl_;
};
}; // namespace hislam
