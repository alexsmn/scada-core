#include "scada/method_util.h"

#include <format>
#include <gmock/gmock.h>

using namespace testing;

namespace scada {

TEST(MethodUtilTest, InvokeMethod) {
  std::string result;
  auto method = [&](int a, std::string b) {
    result = std::format("{}-{}", a, b);
  };
  EXPECT_FALSE(InvokeMethod(std::vector<Variant>{}, method));
  EXPECT_FALSE(InvokeMethod(std::vector<Variant>{111}, method));
  EXPECT_FALSE(InvokeMethod(std::vector<Variant>{"Text", 111}, method));
  EXPECT_TRUE(InvokeMethod(std::vector<Variant>{111, "Text"}, method));
  EXPECT_EQ(result, "111-Text");
}

}  // namespace scada
