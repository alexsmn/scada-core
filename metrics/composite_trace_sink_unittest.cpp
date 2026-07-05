#include "metrics/composite_trace_sink.h"

#include "metrics/tracer.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace metrics {
namespace {

// Records span events; optionally answers GetTraceParent like an exporting
// sink would.
class RecordingSink final : public TraceSink {
 public:
  explicit RecordingSink(std::string trace_parent = {})
      : trace_parent_{std::move(trace_parent)} {}

  void StartSpan(const TraceSpanId& span_id,
                 std::string_view name,
                 const TraceSpanId& parent_span_id,
                 TraceSpanKind kind,
                 std::string_view remote_parent) override {
    started_.emplace_back(name);
    last_remote_parent_ = std::string{remote_parent};
  }

  void EndSpan(const TraceSpanId& span_id) override {
    ended_.push_back(span_id);
  }

  std::string GetTraceParent(const TraceSpanId& span_id) const override {
    return trace_parent_;
  }

  std::vector<std::string> started_;
  std::vector<TraceSpanId> ended_;
  std::string last_remote_parent_;
  std::string trace_parent_;
};

TEST(CompositeTraceSinkTest, FansOutStartAndEnd) {
  RecordingSink first;
  RecordingSink second;
  CompositeTraceSink composite{{&first, &second}};
  Tracer tracer{composite};

  {
    auto span = tracer.StartSpan("Read", TraceSpanKind::kServer, "remote");
  }

  ASSERT_EQ(first.started_, std::vector<std::string>{"Read"});
  ASSERT_EQ(second.started_, std::vector<std::string>{"Read"});
  EXPECT_EQ(first.last_remote_parent_, "remote");
  EXPECT_EQ(first.ended_.size(), 1u);
  EXPECT_EQ(second.ended_.size(), 1u);
  EXPECT_EQ(first.ended_, second.ended_);
}

TEST(CompositeTraceSinkTest, TraceParentReturnsFirstNonEmpty) {
  RecordingSink watchdog;  // Returns empty like the hung-span watchdog.
  RecordingSink exporter{
      "00-0af7651916cd43dd8448eb211c80319c-"
      "b7ad6b7169203331-01"};
  CompositeTraceSink composite{{&watchdog, &exporter}};

  EXPECT_EQ(composite.GetTraceParent("any"), exporter.trace_parent_);
}

TEST(CompositeTraceSinkTest, TraceParentEmptyWhenNoSinkExports) {
  RecordingSink first;
  RecordingSink second;
  CompositeTraceSink composite{{&first, &second}};

  EXPECT_EQ(composite.GetTraceParent("any"), "");
}

}  // namespace
}  // namespace metrics
