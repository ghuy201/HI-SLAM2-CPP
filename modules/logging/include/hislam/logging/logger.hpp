#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace hislam::logging {

enum class Level : std::uint8_t { Info = 0, Warn = 1, Error = 2 };

enum class Color { Default, Red, Green, Yellow, Blue, Magenta, Cyan, White };

class Logger {
public:
  using Clock = std::chrono::steady_clock;
  using Duration = Clock::duration;

  // Useful for ROS, custom GUIs, telemetry, etc.
  using Sink = std::function<void(Level, std::string_view)>;

  explicit Logger(std::string name = "HI-SLAM2") : name_(std::move(name)) {}

  ~Logger() = default;

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  Logger(Logger &&) = delete;
  Logger &operator=(Logger &&) = delete;

  // --------------------------------------------------
  // Configuration
  // --------------------------------------------------

  void setName(std::string name) {
    std::scoped_lock lock(mutex_);
    name_ = std::move(name);
  }

  void setColor(Color color) {
    std::scoped_lock lock(mutex_);
    color_ = color;
  }

  void enableColor(bool enabled) {
    std::scoped_lock lock(mutex_);
    use_color_ = enabled;
  }

  void setLevel(Level level) {
    std::scoped_lock lock(mutex_);
    min_level_ = level;
  }

  // Redirect logs to an arbitrary std::ostream.
  //
  // The caller must ensure that the stream outlives
  // this Logger or until another sink is configured.
  void setOutputStream(std::ostream &stream) {
    std::scoped_lock lock(mutex_);

    output_ = &stream;
    callback_ = nullptr;
  }

  // Replace the output stream with a callback.
  //
  // The callback receives:
  //   Level
  //   Formatted message WITHOUT ANSI escape codes.
  //
  // The callback must not recursively log through
  // this same Logger instance.
  void setCallbackSink(Sink sink) {
    if (!sink) {
      throw std::invalid_argument("Logging callback must not be empty");
    }

    std::scoped_lock lock(mutex_);

    callback_ = std::move(sink);
    output_ = nullptr;
  }

  // Restore standard output.
  void useStdout() { setOutputStream(std::cout); }

  // Optional file output.
  //
  // The file is written IN ADDITION to the main sink.
  // Parent directories are created automatically.
  void setOutputFile(const std::filesystem::path &path, bool append = true) {

    const auto parent = path.parent_path();

    if (!parent.empty()) {
      std::filesystem::create_directories(parent);
    }

    std::ofstream file(path, std::ios::out |
                                 (append ? std::ios::app : std::ios::trunc));

    if (!file.is_open()) {
      throw std::runtime_error("Failed to open log file: " + path.string());
    }

    std::scoped_lock lock(mutex_);
    file_ = std::move(file);
  }

  void disableFileOutput() {
    std::scoped_lock lock(mutex_);

    if (file_.is_open()) {
      file_.close();
    }
  }

  void clearThrottleHistory() {
    std::scoped_lock lock(mutex_);
    last_log_times_.clear();
  }

  // --------------------------------------------------
  // Standard logging
  // --------------------------------------------------

  void log(Level level, std::string_view message) {
    std::scoped_lock lock(mutex_);

    if (level < min_level_) {
      return;
    }

    emit(level, message);
  }

  void info(std::string_view message) { log(Level::Info, message); }

  void warn(std::string_view message) { log(Level::Warn, message); }

  void error(std::string_view message) { log(Level::Error, message); }

  // --------------------------------------------------
  // Rate-limited logging
  // --------------------------------------------------

  void logEvery(
      Level level, Duration interval, std::string_view message,
      const std::source_location &location = std::source_location::current()) {

    if (interval < Duration::zero()) {
      throw std::invalid_argument("Logging interval cannot be negative");
    }

    std::scoped_lock lock(mutex_);

    if (level < min_level_) {
      return;
    }

    const auto key = makeThrottleKey(level, location);
    const auto now = Clock::now();

    const auto it = last_log_times_.find(key);

    if (it != last_log_times_.end()) {
      if (now - it->second < interval) {
        return;
      }
    }

    // Update only when the message is emitted.
    last_log_times_[key] = now;

    emit(level, message);
  }

  void infoEvery(
      Duration interval, std::string_view message,
      const std::source_location &location = std::source_location::current()) {

    logEvery(Level::Info, interval, message, location);
  }

  void warnEvery(
      Duration interval, std::string_view message,
      const std::source_location &location = std::source_location::current()) {

    logEvery(Level::Warn, interval, message, location);
  }

  void errorEvery(
      Duration interval, std::string_view message,
      const std::source_location &location = std::source_location::current()) {

    logEvery(Level::Error, interval, message, location);
  }

private:
  // --------------------------------------------------
  // Formatting helpers
  // --------------------------------------------------

  [[nodiscard]]
  static constexpr std::string_view levelName(Level level) noexcept {

    switch (level) {
    case Level::Info:
      return "INFO";

    case Level::Warn:
      return "WARN";

    case Level::Error:
      return "ERROR";
    }

    return "UNKNOWN";
  }

  [[nodiscard]]
  static constexpr std::string_view colorCode(Color color) noexcept {

    switch (color) {
    case Color::Default:
      return "";

    case Color::Red:
      return "\033[31m";

    case Color::Green:
      return "\033[32m";

    case Color::Yellow:
      return "\033[33m";

    case Color::Blue:
      return "\033[34m";

    case Color::Magenta:
      return "\033[35m";

    case Color::Cyan:
      return "\033[36m";

    case Color::White:
      return "\033[37m";
    }

    return "";
  }

  [[nodiscard]]
  static constexpr std::string_view resetCode() noexcept {
    return "\033[0m";
  }

  [[nodiscard]]
  std::string makePrefix() const {
    return "[ " + name_ + " ]";
  }

  [[nodiscard]]
  static std::string makeThrottleKey(Level level,
                                     const std::source_location &location) {

    std::string key;

    key.reserve(128);

    key += location.file_name();
    key += ':';
    key += std::to_string(location.line());
    key += ':';
    key += std::to_string(location.column());
    key += ':';
    key += std::to_string(static_cast<int>(level));

    return key;
  }

  // --------------------------------------------------
  // Output
  // --------------------------------------------------

  // IMPORTANT:
  // This method must only be called while mutex_
  // is held by the calling thread.
  void emit(Level level, std::string_view message) {
    const auto prefix = makePrefix();

    const std::string suffix =
        " [" + std::string(levelName(level)) + "] " + std::string(message);

    const std::string plain_message = prefix + suffix;

    // Main output stream
    if (output_ != nullptr) {
      const auto color = colorCode(color_);

      if (use_color_ && !color.empty()) {
        // Only color the logger prefix.
        *output_ << color << prefix << resetCode() << suffix << '\n';
      } else {
        *output_ << plain_message << '\n';
      }

      output_->flush();
    }

    // Custom sink (e.g. ROS)
    if (callback_) {
      callback_(level, plain_message);
    }

    // Optional secondary file output
    if (file_.is_open()) {
      file_ << plain_message << '\n';
      file_.flush();
    }
  }

private:
  mutable std::mutex mutex_;

  std::string name_;

  Color color_ = Color::Cyan;
  Level min_level_ = Level::Info;

  bool use_color_ = true;

  // Default sink
  std::ostream *output_ = &std::cout;

  // Optional replacement sink
  Sink callback_;

  // Optional secondary output
  std::ofstream file_;

  // Per-call-site throttle state
  std::unordered_map<std::string, Clock::time_point> last_log_times_;
};

} // namespace hislam::logging
