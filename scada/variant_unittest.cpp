#include "scada/variant.h"
#include "scada/variant_utils.h"

#include <gmock/gmock.h>

using namespace testing;

TEST(VariantTest, ToString16) {
  EXPECT_EQ(scada::Variant::kFalseString, ToString16(scada::Variant{false}));
  EXPECT_EQ(scada::Variant::kTrueString, ToString16(scada::Variant{true}));
  EXPECT_EQ(u"Привет", ToString16(scada::Variant{u"Привет"}));
}

// ConvertVariant<std::wstring>

TEST(VariantTest, ConvertToWstringFromLocalizedText) {
  scada::Variant v{u"hello"};
  std::wstring result;
  EXPECT_TRUE(scada::ConvertVariant(v, result));
  EXPECT_EQ(L"hello", result);
}

TEST(VariantTest, ConvertToWstringFromLocalizedTextNonAscii) {
  scada::Variant v{u"\u041F\u0440\u0438\u0432\u0435\u0442"};  // "Привет"
  std::wstring result;
  EXPECT_TRUE(scada::ConvertVariant(v, result));
  EXPECT_EQ(L"\x041F\x0440\x0438\x0432\x0435\x0442", result);
}

TEST(VariantTest, ConvertToWstringFromEmptyVariant) {
  // EMPTY variant converts to an empty wstring (ToStringHelper clears target).
  scada::Variant v;
  std::wstring result;
  EXPECT_TRUE(scada::ConvertVariant(v, result));
  EXPECT_TRUE(result.empty());
}

TEST(VariantTest, ConvertToWstringFromInt) {
  // Integer variants are formatted as decimal strings via ToStringHelper.
  scada::Variant v{scada::Int32{42}};
  std::wstring result;
  EXPECT_TRUE(scada::ConvertVariant(v, result));
  EXPECT_EQ(L"42", result);
}
