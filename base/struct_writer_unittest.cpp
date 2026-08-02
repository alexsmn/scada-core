#include "base/struct_writer.h"

#include <format>
#include <ostream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace {

// A simple aggregate printed through StructWriter; exercises the std::formatter
// adapter end to end.
struct Point {
  int x = 0;
  int y = 0;
  std::string label;
};

std::ostream& operator<<(std::ostream& stream, const Point& p) {
  StructWriter{stream}.AddField("x", p.x).AddField("y", p.y).AddField("label",
                                                                      p.label);
  return stream;
}

}  // namespace

template <>
struct std::formatter<Point> : OStreamFormatter {};

namespace {

TEST(StructWriterTest, OstreamOutput) {
  std::ostringstream stream;
  stream << Point{1, 2, "p"};
  EXPECT_EQ(stream.str(), "{x: 1, y: 2, label: \"p\"}");
}

TEST(OStreamFormatterTest, MatchesOstreamOutput) {
  EXPECT_EQ(std::format("{}", Point{1, 2, "p"}), "{x: 1, y: 2, label: \"p\"}");
}

TEST(OStreamFormatterTest, EmbedsInLargerFormatString) {
  EXPECT_EQ(std::format("point={}!", Point{3, 4, "q"}),
            "point={x: 3, y: 4, label: \"q\"}!");
}

TEST(OStreamFormatterTest, RejectsNonEmptyFormatSpec) {
  Point p;
  EXPECT_THROW(std::vformat("{:5}", std::make_format_args(p)),
               std::format_error);
}

}  // namespace
