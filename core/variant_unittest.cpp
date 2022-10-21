#include "core/variant.h"

#include <gmock/gmock.h>

TEST(Variant, ToString16) {
  EXPECT_EQ(scada::Variant::kFalseString, ToString16(scada::Variant{false}));
  EXPECT_EQ(scada::Variant::kTrueString, ToString16(scada::Variant{true}));
  EXPECT_EQ(u"Привет", ToString16(scada::Variant{u"Привет"}));
}
