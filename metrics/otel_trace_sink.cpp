#include "metrics/otel_trace_sink.h"

#include "metrics/trace_parent.h"

#include <opentelemetry/trace/span_context.h>
#include <opentelemetry/trace/span_metadata.h>
#include <opentelemetry/trace/span_startoptions.h>

namespace scada::metrics {

namespace {

namespace trace_api = opentelemetry::trace;

trace_api::SpanKind ToOtelSpanKind(TraceSpanKind kind) {
  switch (kind) {
    case TraceSpanKind::kServer:
      return trace_api::SpanKind::kServer;
    case TraceSpanKind::kClient:
      return trace_api::SpanKind::kClient;
    case TraceSpanKind::kInternal:
      return trace_api::SpanKind::kInternal;
  }
  return trace_api::SpanKind::kInternal;
}

trace_api::SpanContext ToRemoteSpanContext(const W3CTraceParent& parent) {
  return trace_api::SpanContext{trace_api::TraceId{parent.trace_id},
                                trace_api::SpanId{parent.span_id},
                                trace_api::TraceFlags{parent.flags},
                                /*is_remote=*/true};
}

}  // namespace

OtelTraceSink::OtelTraceSink(
    opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer)
    : tracer_{std::move(tracer)} {}

OtelTraceSink::~OtelTraceSink() = default;

void OtelTraceSink::StartSpan(const TraceSpanId& span_id,
                              std::string_view name,
                              const TraceSpanId& parent_span_id,
                              TraceSpanKind kind,
                              std::string_view remote_parent) {
  trace_api::StartSpanOptions options;
  options.kind = ToOtelSpanKind(kind);

  std::lock_guard lock{mutex_};

  bool has_local_parent = false;
  if (!parent_span_id.empty()) {
    if (auto i = spans_.find(parent_span_id); i != spans_.end()) {
      options.parent = i->second->GetContext();
      has_local_parent = true;
    }
  }
  if (!has_local_parent) {
    if (auto remote = ParseTraceParent(remote_parent)) {
      options.parent = ToRemoteSpanContext(*remote);
    }
  }

  spans_.insert_or_assign(
      span_id, tracer_->StartSpan(
                   opentelemetry::nostd::string_view{name.data(), name.size()},
                   options));
}

void OtelTraceSink::EndSpan(const TraceSpanId& span_id) {
  SpanPtr span;
  {
    std::lock_guard lock{mutex_};
    auto i = spans_.find(span_id);
    if (i == spans_.end()) {
      return;  // Unknown span (e.g. started before this sink existed).
    }
    span = std::move(i->second);
    spans_.erase(i);
  }
  span->End();
}

void OtelTraceSink::SetSpanAttribute(const TraceSpanId& span_id,
                                     std::string_view key,
                                     std::string_view value) {
  std::lock_guard lock{mutex_};

  auto i = spans_.find(span_id);
  if (i == spans_.end()) {
    return;
  }
  i->second->SetAttribute(
      opentelemetry::nostd::string_view{key.data(), key.size()},
      opentelemetry::nostd::string_view{value.data(), value.size()});
}

std::string OtelTraceSink::GetTraceParent(const TraceSpanId& span_id) const {
  std::lock_guard lock{mutex_};

  auto i = spans_.find(span_id);
  if (i == spans_.end()) {
    return {};
  }

  const trace_api::SpanContext context = i->second->GetContext();
  W3CTraceParent parent;
  const auto trace_id_bytes = context.trace_id().Id();
  const auto span_id_bytes = context.span_id().Id();
  std::copy(trace_id_bytes.begin(), trace_id_bytes.end(),
            parent.trace_id.begin());
  std::copy(span_id_bytes.begin(), span_id_bytes.end(), parent.span_id.begin());
  parent.flags = context.trace_flags().flags();
  return FormatTraceParent(parent);
}

}  // namespace scada::metrics
