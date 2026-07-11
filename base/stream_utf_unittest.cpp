#include "base/stream_utf.h"

#include "base/boost_log.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

#include <ostream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace {

// The UTF-8 encoding of the Cyrillic word "Привет", written as a plain char
// literal (these sources are UTF-8).
constexpr const char* kPrivetUtf8 = "Привет";

TEST(StreamUtfTest, WStringToOstream) {
  std::ostringstream stream;
  stream << std::wstring{L"hello"};
  EXPECT_EQ(stream.str(), "hello");
}

TEST(StreamUtfTest, WStringViewToOstream) {
  std::ostringstream stream;
  stream << std::wstring_view{L"hello"};
  EXPECT_EQ(stream.str(), "hello");
}

TEST(StreamUtfTest, U16StringToOstream) {
  std::ostringstream stream;
  stream << std::u16string{u"hello"};
  EXPECT_EQ(stream.str(), "hello");
}

TEST(StreamUtfTest, U16StringViewToOstream) {
  std::ostringstream stream;
  stream << std::u16string_view{u"hello"};
  EXPECT_EQ(stream.str(), "hello");
}

TEST(StreamUtfTest, WStringTranscodesNonAscii) {
  std::ostringstream stream;
  stream << std::wstring{L"Привет"};
  EXPECT_EQ(stream.str(), kPrivetUtf8);
}

TEST(StreamUtfTest, U16StringTranscodesNonAscii) {
  std::ostringstream stream;
  stream << std::u16string{u"Привет"};
  EXPECT_EQ(stream.str(), kPrivetUtf8);
}

// A chained insertion returns a usable stream reference (regression for the
// derived-stream return-type deduction).
TEST(StreamUtfTest, ChainsWithOtherInserters) {
  std::ostringstream stream;
  stream << "a=" << std::wstring{L"x"} << " b=" << std::u16string{u"y"};
  EXPECT_EQ(stream.str(), "a=x b=y");
}

// The wide-string operators must also resolve for boost::log's
// formatting_ostream (a distinct stream type), which is why they are templated
// on the stream type (wstring) / declared in namespace boost::log (u16string).
TEST(StreamUtfTest, RendersThroughBoostLogFormattingOstream) {
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
  LOG_INFO(logger) << "w=" << std::wstring{L"Привет"} << " u="
                   << std::u16string{u"Привет"};

  sink->flush();
  boost::log::core::get()->remove_sink(sink);

  EXPECT_NE(output.str().find(std::string{"w="} + kPrivetUtf8 + " u=" +
                              kPrivetUtf8),
            std::string::npos)
      << "actual: " << output.str();
}

}  // namespace
