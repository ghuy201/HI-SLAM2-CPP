#include "hislam/config/config.hpp"

#include <yaml-cpp/yaml.h>

#include <stdexcept>
#include <string>
#include <string_view>

namespace hislam::config {

namespace {

// Read a required YAML field and convert it to T.
template <typename T>
T read(const YAML::Node &node, std::string_view key, std::string_view parent) {
  const std::string name{key};
  const std::string path = std::string(parent) + "." + name;

  const YAML::Node value = node[name];

  if (!value.IsDefined() || value.IsNull()) {
    throw std::runtime_error("Missing configuration parameter: " + path);
  }

  if (!value.IsScalar()) {
    throw std::runtime_error("Expected scalar configuration parameter: " +
                             path);
  }

  try {
    return value.as<T>();
  } catch (const YAML::Exception &e) {
    throw std::runtime_error("Invalid configuration parameter '" + path +
                             "': " + e.what());
  }
}

// Read a required YAML mapping.
YAML::Node section(const YAML::Node &node, std::string_view key) {
  const YAML::Node result = node[std::string(key)];

  if (!result.IsDefined() || !result.IsMap()) {
    throw std::runtime_error("Missing or invalid configuration section: " +
                             std::string(key));
  }

  return result;
}

} // namespace

void Config::load(const std::filesystem::path &path) {

  const YAML::Node root = YAML::LoadFile(path.string());

  if (!root.IsMap()) {
    throw std::runtime_error("Configuration root must be a YAML mapping");
  }

  // Dataset
  {
    const auto node = section(root, "Dataset");

    auto &c = dataset;

    c.pcd_downsample = read<int>(node, "pcd_downsample", "Dataset");

    c.pcd_downsample_init = read<int>(node, "pcd_downsample_init", "Dataset");

    c.adaptive_pointsize = read<bool>(node, "adaptive_pointsize", "Dataset");

    c.point_size = read<double>(node, "point_size", "Dataset");

    c.scale_multiplier = read<double>(node, "scale_multiplier", "Dataset");
  }

  // Tracking
  {
    const auto tracking_section = section(root, "Tracking");

    // Motion filter
    {
      const auto node = section(tracking_section, "motion_filter");

      auto &c = tracking.motion_filter;

      c.init_thresh =
          read<double>(node, "init_thresh", "Tracking.motion_filter");

      c.thresh = read<double>(node, "thresh", "Tracking.motion_filter");

      c.skip_blur = read<bool>(node, "skip_blur", "Tracking.motion_filter");
    }

    // Frontend
    {
      const auto node = section(tracking_section, "frontend");

      auto &c = tracking.frontend;

      constexpr auto parent = "Tracking.frontend";

      c.keyframe_thresh = read<double>(node, "keyframe_thresh", parent);

      c.frontend_thresh = read<double>(node, "frontend_thresh", parent);

      c.frontend_window = read<int>(node, "frontend_window", parent);

      c.frontend_radius = read<int>(node, "frontend_radius", parent);

      c.frontend_nms = read<int>(node, "frontend_nms", parent);

      c.mono_depth_alpha = read<double>(node, "mono_depth_alpha", parent);
    }

    // Backend
    {
      const auto node = section(tracking_section, "backend");

      auto &c = tracking.backend;

      constexpr auto parent = "Tracking.backend";

      c.backend_thresh = read<double>(node, "backend_thresh", parent);

      c.backend_radius = read<int>(node, "backend_radius", parent);

      c.backend_nms = read<int>(node, "backend_nms", parent);

      c.covis_thresh = read<double>(node, "covis_thresh", parent);
    }

    // Pose graph bundle adjustment
    {
      const auto node = section(tracking_section, "pgba");

      auto &c = tracking.pgba;

      c.active = read<bool>(node, "active", "Tracking.pgba");

      c.pgba_thresh = read<double>(node, "pgba_thresh", "Tracking.pgba");
    }
  }

  // Training
  {
    const auto node = section(root, "Training");

    auto &c = training;

    constexpr auto parent = "Training";

    c.init_itr_num = read<int>(node, "init_itr_num", parent);

    c.init_gaussian_update = read<int>(node, "init_gaussian_update", parent);

    c.init_gaussian_reset = read<int>(node, "init_gaussian_reset", parent);

    c.init_gaussian_th = read<double>(node, "init_gaussian_th", parent);

    c.init_gaussian_extent = read<double>(node, "init_gaussian_extent", parent);

    c.gaussian_update_every = read<int>(node, "gaussian_update_every", parent);

    c.gaussian_update_offset =
        read<int>(node, "gaussian_update_offset", parent);

    c.gaussian_th = read<double>(node, "gaussian_th", parent);

    c.gaussian_extent = read<double>(node, "gaussian_extent", parent);

    c.gaussian_reset = read<int>(node, "gaussian_reset", parent);

    c.size_threshold = read<int>(node, "size_threshold", parent);

    c.window_size = read<int>(node, "window_size", parent);

    c.rgb_boundary_threshold =
        read<double>(node, "rgb_boundary_threshold", parent);

    c.lambda_dnormal = read<double>(node, "lambda_dnormal", parent);

    c.compensate_exposure = read<bool>(node, "compensate_exposure", parent);
  }

  // Gaussian optimization
  {
    const auto node = section(root, "opt_params");

    auto &c = optimization;

    constexpr auto parent = "opt_params";

    c.pose_lr = read<double>(node, "pose_lr", parent);

    c.position_lr_init = read<double>(node, "position_lr_init", parent);

    c.position_lr_final = read<double>(node, "position_lr_final", parent);

    c.position_lr_max_steps = read<int>(node, "position_lr_max_steps", parent);

    c.feature_lr = read<double>(node, "feature_lr", parent);

    c.opacity_lr = read<double>(node, "opacity_lr", parent);

    c.scaling_lr = read<double>(node, "scaling_lr", parent);

    c.rotation_lr = read<double>(node, "rotation_lr", parent);

    c.exposure_lr = read<double>(node, "exposure_lr", parent);

    c.percent_dense = read<double>(node, "percent_dense", parent);

    c.lambda_dssim = read<double>(node, "lambda_dssim", parent);

    c.densify_grad_threshold =
        read<double>(node, "densify_grad_threshold", parent);
  }

  validate();
}

void Config::validate() const {

  if (dataset.pcd_downsample <= 0) {
    throw std::invalid_argument("Dataset.pcd_downsample must be positive");
  }

  if (dataset.pcd_downsample_init <= 0) {
    throw std::invalid_argument("Dataset.pcd_downsample_init must be positive");
  }

  if (dataset.point_size <= 0.0) {
    throw std::invalid_argument("Dataset.point_size must be positive");
  }

  if (tracking.frontend.frontend_window <= 0) {
    throw std::invalid_argument(
        "Tracking.frontend.frontend_window must be positive");
  }

  if (tracking.frontend.frontend_radius < 0) {
    throw std::invalid_argument(
        "Tracking.frontend.frontend_radius cannot be negative");
  }

  if (training.window_size <= 0) {
    throw std::invalid_argument("Training.window_size must be positive");
  }

  if (training.gaussian_update_every <= 0) {
    throw std::invalid_argument(
        "Training.gaussian_update_every must be positive");
  }

  if (optimization.pose_lr <= 0.0) {
    throw std::invalid_argument("opt_params.pose_lr must be positive");
  }

  if (optimization.position_lr_max_steps <= 0) {
    throw std::invalid_argument(
        "opt_params.position_lr_max_steps must be positive");
  }

  if (optimization.percent_dense < 0.0 || optimization.percent_dense > 1.0) {
    throw std::invalid_argument(
        "opt_params.percent_dense must be between 0 and 1");
  }
}

} // namespace hislam::config
