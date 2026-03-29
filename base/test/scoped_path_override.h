#pragma once

#include "base/path_service.h"

#include <filesystem>

namespace base {

// Overrides a PathService key to a temporary directory for the duration of a
// test. Restores the original path on destruction.
class ScopedPathOverride {
 public:
  explicit ScopedPathOverride(int key) : key_(key) {
    // Save original path.
    PathService::Get(key_, &original_);

    // Create and use a temporary directory.
    temp_dir_ = std::filesystem::temp_directory_path() / "scada_test";
    std::filesystem::create_directories(temp_dir_);
    PathService::Override(key_, temp_dir_);
  }

  ~ScopedPathOverride() {
    PathService::Override(key_, original_);
    std::error_code ec;
    std::filesystem::remove_all(temp_dir_, ec);
  }

  ScopedPathOverride(const ScopedPathOverride&) = delete;
  ScopedPathOverride& operator=(const ScopedPathOverride&) = delete;

  const std::filesystem::path& GetPath() const { return temp_dir_; }

 private:
  int key_;
  std::filesystem::path original_;
  std::filesystem::path temp_dir_;
};

}  // namespace base
