#pragma once

#include <filesystem>
#include <string>

namespace hislam::config {

struct DatasetConfig {
  int pcd_downsample = 64;
  int pcd_downsample_init = 32;
  bool adaptive_pointsize = true;
  double point_size = 0.05;
  double scale_multiplier = 2.0;
};

struct MotionFilterConfig {
  double init_thresh = 4.0;
  double thresh = 2.4;
  bool skip_blur = true;
};

struct FrontendConfig {
  double keyframe_thresh = 4.0;
  double frontend_thresh = 16.0;
  int frontend_window = 25;
  int frontend_radius = 2;
  int frontend_nms = 1;
  double mono_depth_alpha = 0.01;
};

struct BackendConfig {
  double backend_thresh = 22.0;
  int backend_radius = 2;
  int backend_nms = 3;
  double covis_thresh = 0.3;
};

struct PGBAConfig {
  bool active = true;
  double pgba_thresh = 22.0;
};

struct TrackingConfig {
  MotionFilterConfig motion_filter;
  FrontendConfig frontend;
  BackendConfig backend;
  PGBAConfig pgba;
};

struct TrainingConfig {
  int init_itr_num = 1050;
  int init_gaussian_update = 100;
  int init_gaussian_reset = 500;
  double init_gaussian_th = 0.005;
  double init_gaussian_extent = 30.0;

  int gaussian_update_every = 150;
  int gaussian_update_offset = 50;
  double gaussian_th = 0.7;
  double gaussian_extent = 1.0;
  int gaussian_reset = 2001;

  int size_threshold = 20;
  int window_size = 10;

  double rgb_boundary_threshold = 0.01;
  double lambda_dnormal = 0.5;
  bool compensate_exposure = true;
};

struct OptimizationConfig {
  double pose_lr = 0.0001;
  double position_lr_init = 0.00016;
  double position_lr_final = 0.0000016;

  int position_lr_max_steps = 26000;

  double feature_lr = 0.0025;
  double opacity_lr = 0.05;
  double scaling_lr = 0.001;
  double rotation_lr = 0.001;
  double exposure_lr = 0.01;

  double percent_dense = 0.01;
  double lambda_dssim = 0.2;
  double densify_grad_threshold = 0.0002;
};

struct Config {
public:
  explicit Config(const std::filesystem::path &path) { load(path); };
  Config() = default;
  void load(const std::filesystem::path &path);
  void validate() const;

  DatasetConfig dataset;
  TrackingConfig tracking;
  TrainingConfig training;
  OptimizationConfig optimization;
};
} // namespace hislam::config
