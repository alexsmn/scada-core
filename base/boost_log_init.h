#pragma once

#include "base/boost_log.h"

#include <filesystem>

struct BoostLogParams {
  std::filesystem::path path;
  size_t rotation_size = 10485760;
  size_t max_size = 104857600;
  size_t max_files = 1000;
  bool console = false;
  // Minimal severity level for console logging.
  BoostLogSeverity console_log_severity =
#if defined(NDEBUG)
      BoostLogSeverity::debug;
#else
      BoostLogSeverity::info;
#endif
};

void InitBoostLogging(const BoostLogParams& params);
void ShutdownBoostLogging();
