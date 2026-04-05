#include "scada/localized_text.h"

#include <gtest/gtest.h>

TEST(LocalizedTextTest, ToLocalizedTextFromAscii) {
  auto text = scada::ToLocalizedText("hello");
  EXPECT_EQ(u"hello", text);
}

TEST(LocalizedTextTest, ToLocalizedTextFromUtf8) {
  // "Привет" in UTF-8
  auto text = scada::ToLocalizedText(
      "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
  EXPECT_EQ(u"\u041F\u0440\u0438\u0432\u0435\u0442", text);
}

TEST(LocalizedTextTest, ToLocalizedTextFromEmpty) {
  auto text = scada::ToLocalizedText("");
  EXPECT_TRUE(text.empty());
}

TEST(LocalizedTextTest, ToLocalizedTextFromU16StringView) {
  auto text = scada::ToLocalizedText(std::u16string_view(u"world"));
  EXPECT_EQ(u"world", text);
}

TEST(LocalizedTextTest, ToLocalizedTextFromU16String) {
  std::u16string input = u"test";
  const auto& text = scada::ToLocalizedText(input);
  EXPECT_EQ(u"test", text);
  // Identity: should return reference to the same string.
  EXPECT_EQ(&input, &text);
}

TEST(LocalizedTextTest, ToLocalizedTextFromU16StringMove) {
  auto text = scada::ToLocalizedText(std::u16string(u"moved"));
  EXPECT_EQ(u"moved", text);
}

TEST(LocalizedTextTest, ToStringAscii) {
  auto str = ToString(u"hello");
  EXPECT_FALSE(str.empty());
}

TEST(LocalizedTextTest, ToString16Identity) {
  scada::LocalizedText text = u"test";
  EXPECT_EQ(&text, &ToString16(text));
}

TEST(LocalizedTextTest, RoundTripAscii) {
  auto text = scada::ToLocalizedText("hello world");
  auto str = ToString(text);
  // Native MB encoding of ASCII should produce "hello world".
  EXPECT_EQ("hello world", str);
}
