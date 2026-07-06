#pragma once

#include "base/boost_log_init.h"

#include <string>

namespace metrics {

// Settings for the structured (JSON-lines) console log formatter.
struct StructuredLogFormatterOptions {
  // Prepended to the hex trace id in the `logging.googleapis.com/trace`
  // field. Cloud Logging correlates a log entry with its Cloud Trace span
  // only when the value is the full resource name, so the prefix should be
  // "projects/<project-id>/traces/". Empty emits the bare hex id — still
  // ingested, but not linked in the Trace explorer.
  std::string trace_prefix;
};

// Returns a console formatter that renders each record as one JSON object
// per line following the Google Cloud structured-logging convention
// (https://cloud.google.com/logging/docs/structured-logging). JSON-aware
// collectors (GKE, Cloud Run, Ops Agent fluent-bit, and most others:
// fluentd, Datadog, Loki) lift the conventional fields into the log entry:
//  - "severity"  <- record severity (Google names: DEBUG..CRITICAL)
//  - "time"      <- RFC 3339 UTC emit time
//  - "message"   <- the record message text
//  - other record attributes (LOG_TAG values, channel, LineID) become typed
//    payload fields; reserved keys win over same-named attributes
//  - a "TraceParent" attribute (kTraceParentLogAttribute, otel_log_sink.h)
//    becomes logging.googleapis.com/trace / spanId / trace_sampled
// Install via BoostLogParams::console_formatter. Note the Docker `gcplogs`
// driver does NOT parse JSON lines — this pays off only where a JSON-aware
// agent reads stdout.
BoostLogFormatter MakeStructuredLogFormatter(
    StructuredLogFormatterOptions options);

}  // namespace metrics
