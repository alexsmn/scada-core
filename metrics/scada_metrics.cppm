// scada.metrics — named C++20 module facade over the core/metrics headers.
//
// Same design and rules as scada.base (see base/scada_base.cppm and
// docs/cxx-modules.md): headers stay the source of truth, the global module
// fragment includes them, the purview re-exports names with `export using`.
// `export import scada.base;` mirrors scada_metrics's PUBLIC link.
//
// opentelemetry:: names (pulled by otel_traces.h / otel_trace_sink.h /
// otel_logs.h / otel_log_sink.h) are third-party and deliberately not
// exported; TUs using them include the otel headers textually alongside the
// import.

module;

// ---- Global module fragment: headers stay the source of truth ----
#include "metrics/aggregated_metric.h"
#include "metrics/composite_trace_sink.h"
#include "metrics/meter.h"
#include "metrics/metric_registry.h"
#include "metrics/metric_value.h"
#include "metrics/otel_endpoint.h"
#include "metrics/otel_log_sink.h"
#include "metrics/otel_logs.h"
#include "metrics/otel_metrics.h"
#include "metrics/otel_trace_sink.h"
#include "metrics/otel_traces.h"
#include "metrics/trace_attribute_util.h"
#include "metrics/trace_id.h"
#include "metrics/trace_parent.h"
#include "metrics/trace_sink.h"
#include "metrics/trace_sink_impl.h"
#include "metrics/trace_span_kind.h"
#include "metrics/tracer.h"
#include "metrics/tracing.h"

export module scada.metrics;

export import scada.base;

export namespace metrics {

// composite_trace_sink.h / otel_trace_sink.h
using metrics::CompositeTraceSink;
using metrics::OtelTraceSink;

// meter.h
using metrics::Meter;
using metrics::MetricAttributes;

// metric_registry.h
using metrics::MetricCallback;
using metrics::MetricRegistry;
using metrics::MetricSink;
using metrics::MetricTrigger;
using metrics::MetricValue;

// otel_endpoint.h
using metrics::NormalizeGrpcEndpoint;

// otel_logs.h / otel_log_sink.h
using metrics::kTraceParentLogAttribute;
using metrics::OpenTelemetryLogs;
using metrics::OpenTelemetryLogsOptions;
using metrics::OtelLogSinkBackend;
using metrics::ScopedOtelLogSink;

// otel_metrics.h
using metrics::MetricPoint;
using metrics::MetricValueObserver;
using metrics::OpenTelemetryMetrics;
using metrics::OpenTelemetryMetricsOptions;

// otel_traces.h
using metrics::OpenTelemetryTraces;
using metrics::OpenTelemetryTracesOptions;

// trace_attribute_util.h
using metrics::JoinForAttribute;

}  // namespace metrics

export {
  // aggregated_metric.h / metric_value.h
  using ::AggregatedCounter;
  using ::AggregatedMetric;
  using ::MetricValue;
  using ::ToMetricValue;

  // trace_id.h / trace_parent.h / trace_span_kind.h
  using ::FormatTraceParent;
  using ::IsTraceParent;
  using ::ParseTraceParent;
  using ::TraceId;
  using ::TraceSpanId;
  using ::TraceSpanKind;
  using ::W3CTraceParent;

  // trace_sink.h / trace_sink_impl.h / tracer.h / tracing.h
  using ::NoTraceSink;
  using ::Tracer;
  using ::TraceSink;
  using ::TraceSinkImpl;
  using ::TraceSpan;
}  // export
