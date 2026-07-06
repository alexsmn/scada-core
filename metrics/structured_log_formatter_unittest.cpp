#include "metrics/structured_log_formatter.h"

#include "base/boost_log.h"
#include "metrics/otel_log_sink.h"
#include "metrics/trace_parent.h"

#include <boost/json.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

namespace metrics {
namespace {

// Captures the formatter's output through a real console-style ostream sink,
// so records travel the same emit -> format path production uses.
class StructuredLogFormatterTest : public testing::Test {
 protected:
  using OstreamSink = boost::log::sinks::synchronous_sink<
      boost::log::sinks::text_ostream_backend>;

  void InstallFormatter(StructuredLogFormatterOptions options) {
    sink_ = boost::make_shared<OstreamSink>();
    sink_->locked_backend()->add_stream(
        boost::shared_ptr<std::ostream>(&output_, [](std::ostream*) {}));
    sink_->set_formatter(MakeStructuredLogFormatter(std::move(options)));
    boost::log::core::get()->add_sink(sink_);
  }

  void TearDown() override {
    if (sink_)
      boost::log::core::get()->remove_sink(sink_);
  }

  // One parsed JSON object per emitted record.
  std::vector<boost::json::object> Lines() {
    sink_->flush();
    std::vector<boost::json::object> lines;
    std::istringstream input{output_.str()};
    for (std::string line; std::getline(input, line);)
      lines.push_back(boost::json::parse(line).as_object());
    return lines;
  }

  std::ostringstream output_;
  boost::shared_ptr<OstreamSink> sink_;
};

TEST_F(StructuredLogFormatterTest, EmitsGoogleConventionFields) {
  InstallFormatter({});
  BoostLogger logger{boost::log::keywords::channel = "TestChannel"};

  LOG_WARNING(logger) << "hello " << 42;

  const auto lines = Lines();
  ASSERT_EQ(lines.size(), 1u);
  const auto& object = lines[0];
  EXPECT_EQ(object.at("severity"), "WARNING");
  EXPECT_EQ(object.at("message"), "hello 42");
  EXPECT_EQ(object.at("channel"), "TestChannel");
  // RFC 3339 UTC: "YYYY-MM-DDTHH:MM:SS.ffffffZ".
  const auto time = boost::json::value_to<std::string>(object.at("time"));
  EXPECT_EQ(time.size(), 27u);
  EXPECT_EQ(time[10], 'T');
  EXPECT_EQ(time.back(), 'Z');
}

TEST_F(StructuredLogFormatterTest, ForwardsTypedAttributes) {
  InstallFormatter({});
  BoostLogger logger;

  LOG_INFO(logger) << "typed" << LOG_TAG("Count", int64_t{7})
                   << LOG_TAG("Ratio", 0.5) << LOG_TAG("Flag", true)
                   << LOG_TAG("Name", std::string{"edge-1"});

  const auto lines = Lines();
  ASSERT_EQ(lines.size(), 1u);
  const auto& object = lines[0];
  EXPECT_EQ(object.at("severity"), "INFO");
  EXPECT_EQ(object.at("Count"), 7);
  EXPECT_EQ(object.at("Ratio").as_double(), 0.5);
  EXPECT_EQ(object.at("Flag"), true);
  EXPECT_EQ(object.at("Name"), "edge-1");
}

TEST_F(StructuredLogFormatterTest, EmitsCloudTraceCorrelationFields) {
  InstallFormatter({.trace_prefix = "projects/demo/traces/"});
  BoostLogger logger;

  W3CTraceParent trace_parent;
  trace_parent.trace_id = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
                           0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19};
  trace_parent.span_id = {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28};
  trace_parent.flags = 0x01;

  LOG_INFO(logger) << "correlated"
                   << LOG_TAG(kTraceParentLogAttribute,
                              FormatTraceParent(trace_parent));

  const auto lines = Lines();
  ASSERT_EQ(lines.size(), 1u);
  const auto& object = lines[0];
  EXPECT_EQ(object.at("logging.googleapis.com/trace"),
            "projects/demo/traces/0a0b0c0d0e0f10111213141516171819");
  EXPECT_EQ(object.at("logging.googleapis.com/spanId"), "2122232425262728");
  EXPECT_EQ(object.at("logging.googleapis.com/trace_sampled"), true);
  // The carrier is consumed into the trace fields, not duplicated as text.
  EXPECT_FALSE(object.contains(kTraceParentLogAttribute));
}

TEST_F(StructuredLogFormatterTest, ReservedFieldsWinOverAttributes) {
  InstallFormatter({});
  BoostLogger logger;

  LOG_ERROR(logger) << "real message"
                    << LOG_TAG("message", std::string{"spoofed"})
                    << LOG_TAG("severity", std::string{"DEBUG"});

  const auto lines = Lines();
  ASSERT_EQ(lines.size(), 1u);
  EXPECT_EQ(lines[0].at("message"), "real message");
  EXPECT_EQ(lines[0].at("severity"), "ERROR");
}

TEST_F(StructuredLogFormatterTest, EscapesMessageIntoSingleLine) {
  InstallFormatter({});
  BoostLogger logger;

  LOG_INFO(logger) << "line1\nline2 \"quoted\"";

  const auto lines = Lines();
  // The embedded newline is escaped, so the record still parses as ONE line.
  ASSERT_EQ(lines.size(), 1u);
  EXPECT_EQ(lines[0].at("message"), "line1\nline2 \"quoted\"");
}

}  // namespace
}  // namespace metrics
