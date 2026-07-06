// Smoke test for the scada.metrics module facade: names come from
// `import scada.metrics;` only, including base names re-exported through
// `export import scada.base;`.

#include <cstdint>
#include <string>
#include <variant>

#include <gtest/gtest.h>

// Import after the textual includes (AppleClang 21 libc++ merging bug).
import scada.metrics;

namespace scada_metrics_module {
namespace {

TEST(ScadaMetricsModuleSmoke, TraceParentRoundTrip) {
  // Out-of-line functions: proves linkage through the module boundary.
  W3CTraceParent parent;
  parent.trace_id[0] = 0x0a;
  parent.span_id[0] = 0xb7;
  parent.flags = 0x01;
  std::string formatted = FormatTraceParent(parent);
  EXPECT_TRUE(IsTraceParent(formatted));
  auto parsed = ParseTraceParent(formatted);
  ASSERT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed->trace_id, parent.trace_id);
  EXPECT_TRUE(parsed->sampled());
}

TEST(ScadaMetricsModuleSmoke, MetricValueAndKind) {
  MetricValue value = ToMetricValue(42);
  EXPECT_EQ(std::get<std::int64_t>(value), 42);
  TraceSpanKind kind = TraceSpanKind::kClient;
  EXPECT_NE(kind, TraceSpanKind::kServer);
  EXPECT_EQ(metrics::NormalizeGrpcEndpoint("http://host:4317"), "host:4317");

  // Transitive scada.base surface via export import.
  EXPECT_EQ(Format(7), "7");
}

}  // namespace
}  // namespace scada_metrics_module
