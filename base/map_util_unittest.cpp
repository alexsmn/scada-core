#include "base/map_util.h"

#include <gtest/gtest.h>

#include <map>
#include <memory>

namespace {

TEST(MapUtilTest, FindPtrReturnsPointerForValueMap) {
  std::map<int, int> values{{1, 10}};

  int* value = FindPtr(values, 1);
  int* missing = FindPtr(values, 2);

  ASSERT_NE(value, nullptr);
  EXPECT_EQ(*value, 10);
  *value = 20;
  EXPECT_EQ(values[1], 20);
  EXPECT_EQ(missing, nullptr);
}

TEST(MapUtilTest, FindPtrReturnsConstPointerForConstValueMap) {
  const std::map<int, int> values{{1, 10}};

  const int* value = FindPtr(values, 1);
  const int* missing = FindPtr(values, 2);

  ASSERT_NE(value, nullptr);
  EXPECT_EQ(*value, 10);
  EXPECT_EQ(missing, nullptr);
}

TEST(MapUtilTest, FindOrNullPreservesPointerLikeMapBehavior) {
  std::map<int, std::shared_ptr<int>> values{{1, std::make_shared<int>(10)}};

  std::shared_ptr<int> value = FindOrNull(values, 1);
  std::shared_ptr<int> missing = FindOrNull(values, 2);

  ASSERT_NE(value, nullptr);
  EXPECT_EQ(*value, 10);
  EXPECT_EQ(missing, nullptr);
}

}  // namespace
