#include "scada/localized_text.h"

#include <gtest/gtest.h>

TEST(LocalizedTextTest, ToLocalizedTextFromAscii) {
  auto text = scada::ToLocalizedText("hello");
  EXPECT_EQ(scada::LocalizedText{u"hello"}, text);
  EXPECT_TRUE(text.locale.empty());
}

TEST(LocalizedTextTest, ToLocalizedTextFromUtf8) {
  // "Привет" in UTF-8
  auto text = scada::ToLocalizedText(
      "\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
  EXPECT_EQ(u"Привет", text.text);
}

TEST(LocalizedTextTest, ToLocalizedTextFromEmpty) {
  auto text = scada::ToLocalizedText("");
  EXPECT_TRUE(text.empty());
}

TEST(LocalizedTextTest, ToLocalizedTextFromU16StringView) {
  auto text = scada::ToLocalizedText(std::u16string_view(u"world"));
  EXPECT_EQ(u"world", text.text);
}

TEST(LocalizedTextTest, ToLocalizedTextFromU16String) {
  std::u16string input = u"test";
  auto text = scada::ToLocalizedText(input);
  EXPECT_EQ(u"test", text.text);
  EXPECT_TRUE(text.locale.empty());
}

TEST(LocalizedTextTest, ToLocalizedTextFromU16StringMove) {
  auto text = scada::ToLocalizedText(std::u16string(u"moved"));
  EXPECT_EQ(u"moved", text.text);
}

TEST(LocalizedTextTest, ToStringAscii) {
  auto str = ToString(u"hello");
  EXPECT_FALSE(str.empty());
}

TEST(LocalizedTextTest, ToString16ReturnsTextField) {
  scada::LocalizedText text = u"test";
  EXPECT_EQ(&text.text, &ToString16(text));
}

TEST(LocalizedTextTest, RoundTripAscii) {
  auto text = scada::ToLocalizedText("hello world");
  auto str = ToString(text);
  // Native MB encoding of ASCII should produce "hello world".
  EXPECT_EQ("hello world", str);
}

TEST(LocalizedTextTest, LocaleParticipatesInEquality) {
  scada::LocalizedText plain{u"text"};
  scada::LocalizedText localized{"ru", u"text"};
  EXPECT_NE(plain, localized);
  EXPECT_EQ(localized, (scada::LocalizedText{"ru", u"text"}));
}

TEST(LocalizedTextTest, EmptyRequiresBothFieldsEmpty) {
  EXPECT_TRUE(scada::LocalizedText{}.empty());
  EXPECT_FALSE(scada::LocalizedText{u"text"}.empty());
  // A locale-only value is not null: it must survive a codec round-trip.
  EXPECT_FALSE((scada::LocalizedText{"ru", {}}).empty());
}

TEST(LocalizedTextTest, AppendKeepsLocale) {
  scada::LocalizedText text{"ru", u"a"};
  text += u" - ";
  text += std::u16string{u"b"};
  text += char16_t{u'c'};
  text += scada::LocalizedText{"en", u"d"};
  EXPECT_EQ(u"a - bcd", text.text);
  EXPECT_EQ("ru", text.locale);
}

TEST(LocalizedTextTest, ComparisonAgainstLiteralCompiles) {
  scada::LocalizedText text{u"abc"};
  EXPECT_EQ(text, u"abc");
  EXPECT_NE(text, u"abd");
}
