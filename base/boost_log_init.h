#pragma once

#include "base/boost_log.h"

#include <boost/log/core/record_view.hpp>
#include <boost/log/utility/formatting_ostream.hpp>

#include <filesystem>
#include <functional>

// A Boost.Log formatter: renders one record into the sink's character stream.
using BoostLogFormatter =
    std::function<void(const boost::log::record_view& record,
                       boost::log::formatting_ostream& stream)>;

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
  // Overrides the console sink's default human-readable text formatter (the
  // file sink always uses the text formatter). Used for structured JSON-lines
  // stdout (metrics::MakeStructuredLogFormatter) so JSON-aware collectors
  // ingest severity/fields instead of flat text.
  BoostLogFormatter console_formatter;
};

void InitBoostLogging(const BoostLogParams& params);
void ShutdownBoostLogging();
