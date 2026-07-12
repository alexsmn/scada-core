#include "metrics/trace_attribute_util.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace scada::metrics {
namespace {

std::string Identity(const std::string& value) {
  return value;
}

TEST(TraceAttributeUtilTest, JoinsAllWhenUnderCap) {
  const std::vector<std::string> ids{"ns=1;i=1", "ns=1;i=2"};
  EXPECT_EQ(JoinForAttribute(ids, Identity), "ns=1;i=1,ns=1;i=2");
}

TEST(TraceAttributeUtilTest, EmptyRangeYieldsEmpty) {
  const std::vector<std::string> ids;
  EXPECT_EQ(JoinForAttribute(ids, Identity), "");
}

TEST(TraceAttributeUtilTest, CapsLargeBatches) {
  const std::vector<std::string> ids{"a", "b", "c", "d", "e", "f", "g"};
  EXPECT_EQ(JoinForAttribute(ids, Identity), "a,b,c,d,e,+2");
  EXPECT_EQ(JoinForAttribute(ids, Identity, 2), "a,b,+5");
}

TEST(TraceAttributeUtilTest, AppliesProjection) {
  const std::vector<int> ids{7, 8};
  EXPECT_EQ(JoinForAttribute(ids, [](int v) { return std::to_string(v * 2); }),
            "14,16");
}

}  // namespace
}  // namespace scada::metrics
