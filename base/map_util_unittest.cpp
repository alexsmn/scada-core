#include "base/map_util.h"

#include <gtest/gtest.h>

#include <map>
#include <memory>

namespace {

TEST(MapUtilTest, FindReturnsReferenceForValueMap) {
  std::map<int, int> values{{1, 10}};

  boost::optional<int&> value = Find(values, 1);
  boost::optional<int&> missing = Find(values, 2);

  ASSERT_TRUE(value);
  EXPECT_EQ(*value, 10);
  *value = 20;
  EXPECT_EQ(values[1], 20);
  EXPECT_FALSE(missing);
}

TEST(MapUtilTest, FindReturnsConstReferenceForConstValueMap) {
  const std::map<int, int> values{{1, 10}};

  boost::optional<const int&> value = Find(values, 1);
  boost::optional<const int&> missing = Find(values, 2);

  ASSERT_TRUE(value);
  EXPECT_EQ(*value, 10);
  EXPECT_FALSE(missing);
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
