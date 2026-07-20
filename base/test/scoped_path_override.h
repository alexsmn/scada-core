#pragma once

#include "base/path_service.h"

#include <cstdio>
#include <filesystem>
#include <random>

namespace scada::base {

// Overrides a PathService key to a temporary directory for the duration of a
// test. Restores the original path on destruction.
//
// The directory is unique per instance. A fixed name would be shared by every
// test process on the machine, and cleanup runs only in the destructor — so a
// test that panics mid-run would leak its files into every later run (a
// crashed client test once left a saved profile that made unrelated tests
// open its windows and crash the same way). Uniqueness also keeps concurrent
// ctest processes from racing each other, at the cost of leaking an empty
// directory when a test crashes.
class ScopedPathOverride {
 public:
  explicit ScopedPathOverride(int key) : key_(key) {
    // Save original path.
    PathService::Get(key_, &original_);

    // Create and use a unique temporary directory.
    const std::filesystem::path base = std::filesystem::temp_directory_path();
    std::random_device random;
    for (;;) {
      char name[32];
      std::snprintf(name, sizeof(name), "scada_test-%08x", random());
      temp_dir_ = base / name;
      // create_directory returns false iff the directory already exists;
      // retry with a fresh suffix until we own a new one.
      if (std::filesystem::create_directory(temp_dir_))
        break;
    }
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

}  // namespace scada::base
