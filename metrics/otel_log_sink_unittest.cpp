#include "metrics/otel_log_sink.h"

#include "base/boost_log.h"
#include "metrics/otel_logs.h"
#include "metrics/trace_parent.h"

#include <gtest/gtest.h>
#include <opentelemetry/sdk/common/exporter_utils.h>
#include <opentelemetry/sdk/logs/exporter.h>
#include <opentelemetry/sdk/logs/read_write_log_record.h>

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace scada::metrics {
namespace {

namespace logs_sdk = opentelemetry::sdk::logs;

// A plain-data snapshot of one exported record, safe to inspect after the
// SDK recycles its recordables.
struct ExportedRecord {
  opentelemetry::logs::Severity severity =
      opentelemetry::logs::Severity::kInvalid;
  std::string body;
  std::map<std::string, opentelemetry::sdk::common::OwnedAttributeValue>
      attributes;
  std::string trace_id_hex;
  std::string span_id_hex;
  bool sampled = false;
};

// Collects exported log records for assertions. The vcpkg opentelemetry-cpp
// build ships no in-memory log exporter (unlike spans/metrics), so the test
// provides its own; state is shared because the provider takes exporter
// ownership.
class CollectingLogRecordExporter final : public logs_sdk::LogRecordExporter {
 public:
  struct State {
    std::mutex mutex;
    std::vector<ExportedRecord> records;

    std::vector<ExportedRecord> Snapshot() {
      std::lock_guard lock{mutex};
      return records;
    }
  };

  explicit CollectingLogRecordExporter(std::shared_ptr<State> state)
      : state_{std::move(state)} {}

  std::unique_ptr<logs_sdk::Recordable> MakeRecordable() noexcept override {
    return std::make_unique<logs_sdk::ReadWriteLogRecord>();
  }

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::nostd::span<std::unique_ptr<logs_sdk::Recordable>>&
          records) noexcept override {
    std::lock_guard lock{state_->mutex};
    for (const auto& recordable : records) {
      const auto& record =
          static_cast<const logs_sdk::ReadWriteLogRecord&>(*recordable);
      ExportedRecord exported;
      exported.severity = record.GetSeverity();
      if (const auto* body =
              opentelemetry::nostd::get_if<std::string>(&record.GetBody())) {
        exported.body = *body;
      }
      for (const auto& [key, value] : record.GetAttributes())
        exported.attributes.emplace(key, value);

      char trace_id_hex[opentelemetry::trace::TraceId::kSize * 2];
      record.GetTraceId().ToLowerBase16(trace_id_hex);
      exported.trace_id_hex.assign(trace_id_hex, sizeof(trace_id_hex));
      char span_id_hex[opentelemetry::trace::SpanId::kSize * 2];
      record.GetSpanId().ToLowerBase16(span_id_hex);
      exported.span_id_hex.assign(span_id_hex, sizeof(span_id_hex));
      exported.sampled = record.GetTraceFlags().IsSampled();

      state_->records.push_back(std::move(exported));
    }
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }

 private:
  std::shared_ptr<State> state_;
};

class OtelLogSinkTest : public testing::Test {
 protected:
  // Emitted records reach `state_` only after ForceFlush (batch processor).
  std::vector<ExportedRecord> Flush() {
    runtime_.ForceFlush();
    return state_->Snapshot();
  }

  std::shared_ptr<CollectingLogRecordExporter::State> state_{
      std::make_shared<CollectingLogRecordExporter::State>()};
  OpenTelemetryLogs runtime_{
      OpenTelemetryLogsOptions{.service_name = "scada-test",
                               .endpoint = "unused:0"},
      std::make_unique<CollectingLogRecordExporter>(state_)};
};

TEST_F(OtelLogSinkTest, MapsSeverityBodyAndChannel) {
  ScopedOtelLogSink sink{runtime_.logger(), BoostLogSeverity::trace};
  BoostLogger logger{boost::log::keywords::channel = "TestChannel"};

  LOG_WARNING(logger) << "hello " << 42;

  const auto records = Flush();
  ASSERT_EQ(records.size(), 1u);
  EXPECT_EQ(records[0].severity, opentelemetry::logs::Severity::kWarn);
  EXPECT_EQ(records[0].body, "hello 42");
  ASSERT_TRUE(records[0].attributes.contains("channel"));
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(
                records[0].attributes.at("channel")),
            "TestChannel");
}

TEST_F(OtelLogSinkTest, ForwardsTypedAttributes) {
  ScopedOtelLogSink sink{runtime_.logger(), BoostLogSeverity::trace};
  BoostLogger logger;

  LOG_INFO(logger) << "typed" << LOG_TAG("Count", int64_t{7})
                   << LOG_TAG("Ratio", 0.5) << LOG_TAG("Flag", true)
                   << LOG_TAG("Name", std::string{"edge-1"});

  const auto records = Flush();
  ASSERT_EQ(records.size(), 1u);
  const auto& attributes = records[0].attributes;
  EXPECT_EQ(opentelemetry::nostd::get<int64_t>(attributes.at("Count")), 7);
  EXPECT_EQ(opentelemetry::nostd::get<double>(attributes.at("Ratio")), 0.5);
  EXPECT_EQ(opentelemetry::nostd::get<bool>(attributes.at("Flag")), true);
  EXPECT_EQ(opentelemetry::nostd::get<std::string>(attributes.at("Name")),
            "edge-1");
}

TEST_F(OtelLogSinkTest, CorrelatesTraceParentAttribute) {
  ScopedOtelLogSink sink{runtime_.logger(), BoostLogSeverity::trace};
  BoostLogger logger;

  W3CTraceParent trace_parent;
  trace_parent.trace_id = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
                           0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19};
  trace_parent.span_id = {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28};
  trace_parent.flags = 0x01;

  LOG_INFO(logger) << "correlated"
                   << LOG_TAG(kTraceParentLogAttribute,
                              FormatTraceParent(trace_parent));

  const auto records = Flush();
  ASSERT_EQ(records.size(), 1u);
  EXPECT_EQ(records[0].trace_id_hex, "0a0b0c0d0e0f10111213141516171819");
  EXPECT_EQ(records[0].span_id_hex, "2122232425262728");
  EXPECT_TRUE(records[0].sampled);
  // The carrier is consumed into the trace fields, not duplicated as text.
  EXPECT_FALSE(records[0].attributes.contains(kTraceParentLogAttribute));
}

TEST_F(OtelLogSinkTest, FiltersBelowMinSeverity) {
  ScopedOtelLogSink sink{runtime_.logger(), BoostLogSeverity::info};
  BoostLogger logger;

  LOG_TRACE(logger) << "dropped";
  LOG_ERROR(logger) << "exported";

  const auto records = Flush();
  ASSERT_EQ(records.size(), 1u);
  EXPECT_EQ(records[0].body, "exported");
  EXPECT_EQ(records[0].severity, opentelemetry::logs::Severity::kError);
}

TEST_F(OtelLogSinkTest, StopsExportingAfterScopeExit) {
  BoostLogger logger;
  {
    ScopedOtelLogSink sink{runtime_.logger(), BoostLogSeverity::trace};
    LOG_INFO(logger) << "inside";
  }
  LOG_INFO(logger) << "outside";

  const auto records = Flush();
  ASSERT_EQ(records.size(), 1u);
  EXPECT_EQ(records[0].body, "inside");
}

}  // namespace
}  // namespace scada::metrics
