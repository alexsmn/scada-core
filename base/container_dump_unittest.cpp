#include "base/container_dump.h"

#include "base/boost_log.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

#include <format>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace base {
namespace {

// The wrappers must render identically through std::format and through a
// std::ostream, matching the historical operator<< output format.

TEST(ContainerDumpTest, ListFormatsAsBracketedCommaSeparated) {
  const std::vector<int> v{1, 2, 3};
  EXPECT_EQ(std::format("{}", AsList(v)), "[1, 2, 3]");

  std::ostringstream stream;
  stream << AsList(v);
  EXPECT_EQ(stream.str(), "[1, 2, 3]");
}

TEST(ContainerDumpTest, EmptyListIsEmptyBrackets) {
  const std::vector<int> v;
  EXPECT_EQ(std::format("{}", AsList(v)), "[]");
}

TEST(ContainerDumpTest, DictFormatsAsBraceColonSeparated) {
  const std::map<std::string, int> m{{"a", 1}, {"b", 2}};
  // std::map iterates in key order.
  EXPECT_EQ(std::format("{}", AsDict(m)), "{a: 1, b: 2}");

  std::ostringstream stream;
  stream << AsDict(m);
  EXPECT_EQ(stream.str(), "{a: 1, b: 2}");
}

TEST(ContainerDumpTest, EmptyDictIsEmptyBraces) {
  const std::map<std::string, int> m;
  EXPECT_EQ(std::format("{}", AsDict(m)), "{}");
}

TEST(ContainerDumpTest, OptionalWithValueRendersValue) {
  const std::optional<int> o = 42;
  EXPECT_EQ(std::format("{}", AsOpt(o)), "42");

  std::ostringstream stream;
  stream << AsOpt(o);
  EXPECT_EQ(stream.str(), "42");
}

TEST(ContainerDumpTest, EmptyOptionalRendersNullopt) {
  const std::optional<int> o;
  EXPECT_EQ(std::format("{}", AsOpt(o)), "nullopt");

  std::ostringstream stream;
  stream << AsOpt(o);
  EXPECT_EQ(stream.str(), "nullopt");
}

TEST(ContainerDumpTest, ListRendersStringElements) {
  const std::vector<std::string> v{"x", "y"};
  EXPECT_EQ(std::format("{}", AsList(v)), "[x, y]");
}

TEST(ContainerDumpTest, RejectsNonEmptyFormatSpec) {
  const std::vector<int> v{1};
  ListDump<std::vector<int>> w = AsList(v);
  EXPECT_THROW(std::vformat("{:5}", std::make_format_args(w)),
               std::format_error);
}

// Streaming a dump wrapper into a LOG record exercises the
// boost::log::formatting_ostream path (a distinct stream type) that the old
// global container operator<< overloads never reached. Capture the record
// message through a real sink and assert it renders.
TEST(ContainerDumpTest, RendersThroughBoostLogFormattingOstream) {
  using OstreamSink = boost::log::sinks::synchronous_sink<
      boost::log::sinks::text_ostream_backend>;
  std::ostringstream output;
  boost::shared_ptr<OstreamSink> sink = boost::make_shared<OstreamSink>();
  sink->locked_backend()->add_stream(
      boost::shared_ptr<std::ostream>(&output, [](std::ostream*) {}));
  sink->set_formatter(boost::log::expressions::stream
                      << boost::log::expressions::smessage);
  boost::log::core::get()->add_sink(sink);

  BoostLogger logger;
  const std::vector<int> v{1, 2, 3};
  const std::map<std::string, int> m{{"a", 1}};
  LOG_INFO(logger) << "ids=" << AsList(v) << " dict=" << AsDict(m);

  sink->flush();
  boost::log::core::get()->remove_sink(sink);

  EXPECT_NE(output.str().find("ids=[1, 2, 3] dict={a: 1}"), std::string::npos)
      << "actual: " << output.str();
}

}  // namespace
}  // namespace base
