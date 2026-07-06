#pragma once

#include "base/boost_log.h"

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/unlocked_frontend.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <opentelemetry/logs/logger.h>
#include <opentelemetry/nostd/shared_ptr.h>

namespace metrics {

// Boost.Log record attribute that carries a W3C traceparent string
// (trace_parent.h). When present, the OTel sink parses it and stamps the
// exported log record with the trace/span id so log backends (e.g. Cloud
// Logging) correlate the entry with the matching distributed-trace span.
// Attach it at the log site with
// `LOG_TAG(metrics::kTraceParentLogAttribute, FormatTraceParent(...))` or
// scope-wide with LOG_SCOPED_TAG / LOG_BIND_TAG.
inline constexpr const char* kTraceParentLogAttribute = "TraceParent";

// Boost.Log sink backend bridging log records into the OpenTelemetry Logs
// API. There is no first-party Boost.Log appender in opentelemetry-cpp; this
// backend is the bridge the OTel Logs API is designed for. It preserves the
// record structure that the text formatters flatten (boost_log_init.cpp):
//  - Severity        -> OTel severity (trace..fatal map 1:1)
//  - Message         -> body
//  - Channel         -> "channel" attribute
//  - LOG_TAG values  -> typed attributes (bool / int64 / uint64 / double /
//                       string; wide strings are UTF-8 converted)
//  - "TraceParent"   -> trace_id / span_id / flags (see above)
// The Boost "TimeStamp" attribute is a local-clock ptime; instead of a
// timezone-sensitive conversion the backend stamps system_clock::now(),
// which is exact because consume() runs synchronously at the emit site.
//
// Feeding is concurrent: EmitLogRecord hands the populated record to the
// (internally synchronized) processor, so the backend needs no frontend
// locking — pair it with sinks::unlocked_sink as ScopedOtelLogSink does.
class OtelLogSinkBackend final
    : public boost::log::sinks::basic_sink_backend<
          boost::log::sinks::concurrent_feeding> {
 public:
  explicit OtelLogSinkBackend(
      opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> logger);

  void consume(const boost::log::record_view& record);

 private:
  const opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> logger_;
};

// Installs an OtelLogSinkBackend on the global Boost.Log core for its own
// lifetime: every record at or above `min_severity` emitted anywhere in the
// process is exported through `logger`'s provider. Keep the owning
// OpenTelemetryLogs runtime alive for at least as long as this sink.
class ScopedOtelLogSink {
 public:
  ScopedOtelLogSink(
      opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> logger,
      BoostLogSeverity min_severity);
  ~ScopedOtelLogSink();

  ScopedOtelLogSink(const ScopedOtelLogSink&) = delete;
  ScopedOtelLogSink& operator=(const ScopedOtelLogSink&) = delete;

 private:
  boost::shared_ptr<boost::log::sinks::unlocked_sink<OtelLogSinkBackend>>
      sink_;
};

}  // namespace metrics
