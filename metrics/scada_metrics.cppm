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
#include "metrics/boost_log_attribute_types.h"
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
#include "metrics/structured_log_formatter.h"
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
using scada::metrics::CompositeTraceSink;
using scada::metrics::OtelTraceSink;

// meter.h
using scada::metrics::Meter;
using scada::metrics::MetricAttributes;

// metric_registry.h
using scada::metrics::MetricCallback;
using scada::metrics::MetricRegistry;
using scada::metrics::MetricSink;
using scada::metrics::MetricTrigger;
using scada::metrics::MetricValue;

// otel_endpoint.h
using scada::metrics::NormalizeGrpcEndpoint;

// otel_logs.h / otel_log_sink.h
using scada::metrics::kTraceParentLogAttribute;
using scada::metrics::OpenTelemetryLogs;
using scada::metrics::OpenTelemetryLogsOptions;
using scada::metrics::OtelLogSinkBackend;
using scada::metrics::ScopedOtelLogSink;

// boost_log_attribute_types.h / structured_log_formatter.h
using scada::metrics::BoostLogAttributeTypes;
using scada::metrics::MakeStructuredLogFormatter;
using scada::metrics::StructuredLogFormatterOptions;

// otel_metrics.h
using scada::metrics::MetricPoint;
using scada::metrics::MetricValueObserver;
using scada::metrics::OpenTelemetryMetrics;
using scada::metrics::OpenTelemetryMetricsOptions;

// otel_traces.h
using scada::metrics::OpenTelemetryTraces;
using scada::metrics::OpenTelemetryTracesOptions;

// trace_attribute_util.h
using scada::metrics::JoinForAttribute;

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
