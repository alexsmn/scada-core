#include "metrics/tracing.h"

#include "base/check.h"
#include "metrics/trace_sink.h"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace {

TraceSpanId GenerateTraceSpanId() {
  boost::uuids::random_generator gen;
  auto uuid = gen();
  return boost::uuids::to_string(uuid);
}

}  // namespace

// TraceSpan

TraceSpan::TraceSpan(TraceSink& sink, std::string_view name)
    : TraceSpan{sink,
                /*span_id=*/GenerateTraceSpanId(),
                name,
                /*parent_span_id=*/{},
                TraceSpanKind::kInternal,
                /*remote_parent=*/{}} {}

TraceSpan::TraceSpan(TraceSink& sink,
                     std::string_view name,
                     TraceSpanKind kind,
                     std::string_view remote_parent)
    : TraceSpan{sink,
                /*span_id=*/GenerateTraceSpanId(),
                name,
                /*parent_span_id=*/{},
                kind,
                remote_parent} {}

TraceSpan::TraceSpan(TraceSink& sink,
                     const TraceSpanId& span_id,
                     std::string_view name,
                     const TraceSpanId& parent_span_id,
                     TraceSpanKind kind,
                     std::string_view remote_parent)
    : sink_{&sink}, span_id_{span_id} {
  sink.StartSpan(span_id, name, parent_span_id, kind, remote_parent);
}

TraceSpan::~TraceSpan() {
  if (sink_) {
    sink_->EndSpan(span_id_);
  }
}

std::string TraceSpan::traceparent() const {
  return sink_ ? sink_->GetTraceParent(span_id_) : std::string{};
}

void TraceSpan::SetAttribute(std::string_view key, std::string_view value) {
  if (sink_) {
    sink_->SetSpanAttribute(span_id_, key, value);
  }
}

TraceSpan TraceSpan::StartSpan(std::string_view name) const {
  scada::base::Check(sink_);
  return TraceSpan{*sink_,
                   /*span_id=*/GenerateTraceSpanId(),
                   name,
                   /*parent_span_id=*/span_id_,
                   TraceSpanKind::kInternal,
                   /*remote_parent=*/{}};
}
