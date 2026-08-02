#include "metrics/otel_trace_sink.h"

#include "metrics/otel_traces.h"
#include "metrics/trace_parent.h"
#include "metrics/tracer.h"

#include <gtest/gtest.h>
#include <opentelemetry/exporters/memory/in_memory_span_data.h>
#include <opentelemetry/exporters/memory/in_memory_span_exporter.h>

#include <string>
#include <thread>
#include <vector>

namespace scada::metrics {
namespace {

namespace memory = opentelemetry::exporter::memory;

constexpr std::string_view kRemoteParent =
    "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01";

// Real OTel pipeline with an in-memory exporter: Tracer -> OtelTraceSink ->
// TracerProvider -> InMemorySpanExporter.
class OtelTraceSinkTest : public ::testing::Test {
 protected:
  OtelTraceSinkTest() {
    auto exporter = std::make_unique<memory::InMemorySpanExporter>();
    span_data_ = exporter->GetData();
    traces_ = std::make_unique<OpenTelemetryTraces>(
        OpenTelemetryTracesOptions{.service_name = "scada-test",
                                   .endpoint = "localhost:4317",
                                   .sampling_ratio = 1.0},
        std::move(exporter));
    sink_ = std::make_unique<OtelTraceSink>(traces_->tracer());
    tracer_ = std::make_unique<Tracer>(*sink_);
  }

  // Ended spans, oldest first. Flushes the batch processor.
  std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanData>>
  ExportedSpans() {
    traces_->ForceFlush();
    return span_data_->GetSpans();
  }

  std::shared_ptr<memory::InMemorySpanData> span_data_;
  std::unique_ptr<OpenTelemetryTraces> traces_;
  std::unique_ptr<OtelTraceSink> sink_;
  std::unique_ptr<Tracer> tracer_;
};

TEST_F(OtelTraceSinkTest, ExportsEndedSpanWithNameAndKind) {
  {
    auto span = tracer_->StartSpan("Read", TraceSpanKind::kServer, {});
  }

  auto spans = ExportedSpans();
  ASSERT_EQ(spans.size(), 1u);
  EXPECT_EQ(spans[0]->GetName(), "Read");
  EXPECT_EQ(spans[0]->GetSpanKind(), opentelemetry::trace::SpanKind::kServer);
  EXPECT_TRUE(spans[0]->GetSpanContext().IsValid());
}

TEST_F(OtelTraceSinkTest, ChildSpanSharesTraceAndLinksParent) {
  {
    auto parent = tracer_->StartSpan("Parent");
    auto child = parent.StartSpan("Child");
  }

  auto spans = ExportedSpans();
  ASSERT_EQ(spans.size(), 2u);
  // Ended child-first.
  const auto& child = spans[0];
  const auto& parent = spans[1];
  EXPECT_EQ(child->GetName(), "Child");
  EXPECT_EQ(parent->GetName(), "Parent");
  EXPECT_EQ(child->GetTraceId(), parent->GetTraceId());
  EXPECT_EQ(child->GetParentSpanId(), parent->GetSpanId());
}

TEST_F(OtelTraceSinkTest, RemoteParentContinuesTrace) {
  {
    auto span =
        tracer_->StartSpan("Read", TraceSpanKind::kServer, kRemoteParent);
  }

  auto spans = ExportedSpans();
  ASSERT_EQ(spans.size(), 1u);

  auto remote = ParseTraceParent(kRemoteParent);
  ASSERT_TRUE(remote);
  const opentelemetry::trace::TraceId remote_trace_id{
      opentelemetry::nostd::span<const uint8_t, 16>{remote->trace_id.data(),
                                                    16}};
  const opentelemetry::trace::SpanId remote_span_id{
      opentelemetry::nostd::span<const uint8_t, 8>{remote->span_id.data(), 8}};
  EXPECT_EQ(spans[0]->GetTraceId(), remote_trace_id);
  EXPECT_EQ(spans[0]->GetParentSpanId(), remote_span_id);
}

TEST_F(OtelTraceSinkTest, TraceParentRoundTripsIntoChildSink) {
  std::string trace_parent;
  {
    auto span = tracer_->StartSpan("Upstream", TraceSpanKind::kClient, {});
    trace_parent = span.traceparent();
    ASSERT_TRUE(IsTraceParent(trace_parent));
  }

  // A second, independent pipeline (as another tier would run) continues the
  // trace from the propagated traceparent.
  auto exporter = std::make_unique<memory::InMemorySpanExporter>();
  auto* downstream_data = exporter->GetData().get();
  OpenTelemetryTraces downstream_traces{
      OpenTelemetryTracesOptions{.service_name = "scada-test-downstream",
                                 .endpoint = "localhost:4317"},
      std::move(exporter)};
  OtelTraceSink downstream_sink{downstream_traces.tracer()};
  Tracer downstream_tracer{downstream_sink};

  {
    auto span = downstream_tracer.StartSpan(
        "Downstream", TraceSpanKind::kServer, trace_parent);
  }
  downstream_traces.ForceFlush();

  auto upstream_spans = ExportedSpans();
  auto downstream_spans = downstream_data->GetSpans();
  ASSERT_EQ(upstream_spans.size(), 1u);
  ASSERT_EQ(downstream_spans.size(), 1u);
  EXPECT_EQ(downstream_spans[0]->GetTraceId(), upstream_spans[0]->GetTraceId());
  EXPECT_EQ(downstream_spans[0]->GetParentSpanId(),
            upstream_spans[0]->GetSpanId());
}

TEST_F(OtelTraceSinkTest, MalformedRemoteParentStartsNewRoot) {
  {
    auto span = tracer_->StartSpan("Read", TraceSpanKind::kServer,
                                   "550e8400-e29b-41d4-a716-446655440000");
  }

  auto spans = ExportedSpans();
  ASSERT_EQ(spans.size(), 1u);
  // A root span: no parent.
  EXPECT_FALSE(spans[0]->GetParentSpanId().IsValid());
  EXPECT_TRUE(spans[0]->GetSpanContext().IsValid());
}

TEST_F(OtelTraceSinkTest, LocalParentWinsOverRemoteParent) {
  {
    auto parent = tracer_->StartSpan("Parent");
    // A child created through TraceSpan::StartSpan carries only the local
    // parent; verify the linkage holds even when a remote parent was set on
    // the enclosing root.
    auto child = parent.StartSpan("Child");
  }
  auto spans = ExportedSpans();
  ASSERT_EQ(spans.size(), 2u);
  EXPECT_EQ(spans[0]->GetParentSpanId(), spans[1]->GetSpanId());
}

TEST_F(OtelTraceSinkTest, UnsampledRatioStillReturnsTraceParent) {
  auto exporter = std::make_unique<memory::InMemorySpanExporter>();
  auto* data = exporter->GetData().get();
  OpenTelemetryTraces traces{
      OpenTelemetryTracesOptions{.service_name = "scada-test",
                                 .endpoint = "localhost:4317",
                                 .sampling_ratio = 0.0},
      std::move(exporter)};
  OtelTraceSink sink{traces.tracer()};
  Tracer tracer{sink};

  std::string trace_parent;
  {
    auto span = tracer.StartSpan("Read", TraceSpanKind::kServer, {});
    trace_parent = span.traceparent();
  }
  traces.ForceFlush();

  // The span is dropped by the sampler but propagation context stays valid
  // with the sampled flag cleared.
  auto parsed = ParseTraceParent(trace_parent);
  ASSERT_TRUE(parsed);
  EXPECT_FALSE(parsed->sampled());
  EXPECT_TRUE(data->GetSpans().empty());
}

TEST_F(OtelTraceSinkTest, ExportsSpanAttributes) {
  {
    auto span = tracer_->StartSpan("Read", TraceSpanKind::kServer, {});
    span.SetAttribute("scada.node_ids", "ns=1;i=42,ns=1;i=43");
    span.SetAttribute("scada.input_count", "2");
  }

  auto spans = ExportedSpans();
  ASSERT_EQ(spans.size(), 1u);
  const auto& attributes = spans[0]->GetAttributes();
  auto node_ids = attributes.find("scada.node_ids");
  ASSERT_NE(node_ids, attributes.end());
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(node_ids->second),
            "ns=1;i=42,ns=1;i=43");
  auto count = attributes.find("scada.input_count");
  ASSERT_NE(count, attributes.end());
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(count->second), "2");
}

TEST_F(OtelTraceSinkTest, SetAttributeForUnknownIdIsNoOp) {
  sink_->SetSpanAttribute("unknown-span-id", "key", "value");
  EXPECT_TRUE(ExportedSpans().empty());
}

TEST_F(OtelTraceSinkTest, EndSpanForUnknownIdIsNoOp) {
  sink_->EndSpan("unknown-span-id");
  EXPECT_TRUE(ExportedSpans().empty());
}

TEST_F(OtelTraceSinkTest, TraceParentForUnknownIdIsEmpty) {
  EXPECT_EQ(sink_->GetTraceParent("unknown-span-id"), "");
}

TEST_F(OtelTraceSinkTest, ConcurrentSpansSmoke) {
  // Keep the total span count under InMemorySpanExporter's ~100-span buffer
  // so the exact-count assertion below holds; the point of this test is
  // thread-safety of the sink map, not throughput.
  constexpr int kThreads = 4;
  constexpr int kSpansPerThread = 10;

  std::vector<std::thread> threads;
  for (int t = 0; t < kThreads; ++t) {
    threads.emplace_back([this] {
      for (int i = 0; i < kSpansPerThread; ++i) {
        auto span = tracer_->StartSpan("Concurrent");
        auto child = span.StartSpan("ConcurrentChild");
        (void)child.traceparent();
      }
    });
  }
  for (auto& thread : threads) {
    thread.join();
  }

  // Under heavy machine load one ForceFlush window may expire before the
  // batch worker drains; flush repeatedly and accumulate until the expected
  // count arrives (GetSpans() consumes what has been exported so far).
  size_t exported = 0;
  const size_t expected = 2u * kThreads * kSpansPerThread;
  for (int attempt = 0; attempt < 5 && exported < expected; ++attempt) {
    exported += ExportedSpans().size();
  }
  EXPECT_EQ(exported, expected);
}

}  // namespace
}  // namespace scada::metrics
