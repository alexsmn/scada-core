#include "base/string_util.h"

#include <gtest/gtest.h>

#include <string>

// SplitString tests

TEST(SplitStringTest, EmptyString) {
  auto parts = SplitString("", ".");
  EXPECT_TRUE(parts.empty());
}

TEST(SplitStringTest, NoDelimiter) {
  auto parts = SplitString("hello", ".");
  ASSERT_EQ(1u, parts.size());
  EXPECT_EQ("hello", parts[0]);
}

TEST(SplitStringTest, SingleDelimiter) {
  auto parts = SplitString("a.b", ".");
  ASSERT_EQ(2u, parts.size());
  EXPECT_EQ("a", parts[0]);
  EXPECT_EQ("b", parts[1]);
}

TEST(SplitStringTest, MultipleDelimiters) {
  auto parts = SplitString("a.b.c.d", ".");
  ASSERT_EQ(4u, parts.size());
  EXPECT_EQ("a", parts[0]);
  EXPECT_EQ("b", parts[1]);
  EXPECT_EQ("c", parts[2]);
  EXPECT_EQ("d", parts[3]);
}

TEST(SplitStringTest, MultiCharDelimiter) {
  auto parts = SplitString("a::b::c", "::");
  ASSERT_EQ(3u, parts.size());
  EXPECT_EQ("a", parts[0]);
  EXPECT_EQ("b", parts[1]);
  EXPECT_EQ("c", parts[2]);
}

TEST(SplitStringTest, EmptyParts) {
  auto parts = SplitString("a..b", ".");
  ASSERT_EQ(3u, parts.size());
  EXPECT_EQ("a", parts[0]);
  EXPECT_EQ("", parts[1]);
  EXPECT_EQ("b", parts[2]);
}

TEST(SplitStringTest, TrailingDelimiter) {
  auto parts = SplitString("a.b.", ".");
  ASSERT_EQ(3u, parts.size());
  EXPECT_EQ("a", parts[0]);
  EXPECT_EQ("b", parts[1]);
  EXPECT_EQ("", parts[2]);
}

TEST(SplitStringTest, CharDelimiter) {
  auto parts = SplitString("a,b,c", ',');
  ASSERT_EQ(3u, parts.size());
  EXPECT_EQ("a", parts[0]);
  EXPECT_EQ("b", parts[1]);
  EXPECT_EQ("c", parts[2]);
}

// JoinStrings tests

TEST(JoinStringsTest, Empty) {
  std::vector<std::string_view> parts;
  EXPECT_EQ("", JoinStrings(parts, ","));
}

TEST(JoinStringsTest, SingleElement) {
  std::vector<std::string_view> parts = {"hello"};
  EXPECT_EQ("hello", JoinStrings(parts, ","));
}

TEST(JoinStringsTest, MultipleElements) {
  std::vector<std::string_view> parts = {"a", "b", "c"};
  EXPECT_EQ("a,b,c", JoinStrings(parts, ","));
}

TEST(JoinStringsTest, MultiCharDelimiter) {
  std::vector<std::string_view> parts = {"a", "b", "c"};
  EXPECT_EQ("a :: b :: c", JoinStrings(parts, " :: "));
}

TEST(JoinStringsTest, EmptyDelimiter) {
  std::vector<std::string_view> parts = {"a", "b", "c"};
  EXPECT_EQ("abc", JoinStrings(parts, ""));
}

// IsStringASCII tests

TEST(IsStringASCIITest, EmptyString) {
  EXPECT_TRUE(IsStringASCII(""));
}

TEST(IsStringASCIITest, ASCIIString) {
  EXPECT_TRUE(IsStringASCII("Hello, World!"));
}

TEST(IsStringASCIITest, AllPrintableASCII) {
  EXPECT_TRUE(IsStringASCII(
      " !\"#$%&'()*+,-./0123456789:;<=>?@"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
      "abcdefghijklmnopqrstuvwxyz{|}~"));
}

TEST(IsStringASCIITest, ControlChars) {
  EXPECT_TRUE(IsStringASCII(std::string_view("\0\x01\x7f", 3)));
}

TEST(IsStringASCIITest, NonASCII) {
  EXPECT_FALSE(IsStringASCII("\x80"));
  EXPECT_FALSE(IsStringASCII("\xff"));
  EXPECT_FALSE(IsStringASCII("hello\x80world"));
}

TEST(IsStringASCIITest, LongASCIIString) {
  // Test string longer than batch size to exercise word-aligned path.
  std::string long_ascii(1000, 'x');
  EXPECT_TRUE(IsStringASCII(long_ascii));
}

TEST(IsStringASCIITest, LongNonASCIIString) {
  std::string long_str(1000, 'x');
  long_str[999] = '\x80';
  EXPECT_FALSE(IsStringASCII(long_str));
}

// ReplaceSubstringsAfterOffset (std::string) tests

TEST(ReplaceSubstringsAfterOffsetTest, StringBasic) {
  std::string str = "hello world hello";
  ReplaceSubstringsAfterOffset(&str, 0, "hello", "hi");
  EXPECT_EQ("hi world hi", str);
}

TEST(ReplaceSubstringsAfterOffsetTest, StringWithOffset) {
  std::string str = "hello world hello";
  ReplaceSubstringsAfterOffset(&str, 6, "hello", "hi");
  EXPECT_EQ("hello world hi", str);
}

TEST(ReplaceSubstringsAfterOffsetTest, StringNoMatch) {
  std::string str = "hello world";
  ReplaceSubstringsAfterOffset(&str, 0, "xyz", "abc");
  EXPECT_EQ("hello world", str);
}

TEST(ReplaceSubstringsAfterOffsetTest, StringEmptyFind) {
  std::string str = "hello";
  ReplaceSubstringsAfterOffset(&str, 0, "", "xyz");
  EXPECT_EQ("hello", str);
}

TEST(ReplaceSubstringsAfterOffsetTest, StringReplaceWithLonger) {
  std::string str = "a.b.c";
  ReplaceSubstringsAfterOffset(&str, 0, ".", "::");
  EXPECT_EQ("a::b::c", str);
}

TEST(ReplaceSubstringsAfterOffsetTest, StringReplaceWithEmpty) {
  std::string str = "a.b.c";
  ReplaceSubstringsAfterOffset(&str, 0, ".", "");
  EXPECT_EQ("abc", str);
}

// ReplaceSubstringsAfterOffset (std::u16string) tests

TEST(ReplaceSubstringsAfterOffsetTest, U16StringBasic) {
  std::u16string str = u"hello world hello";
  ReplaceSubstringsAfterOffset(&str, 0, u"hello", u"hi");
  EXPECT_EQ(u"hi world hi", str);
}

TEST(ReplaceSubstringsAfterOffsetTest, U16StringWithOffset) {
  std::u16string str = u"hello world hello";
  ReplaceSubstringsAfterOffset(&str, 6, u"hello", u"hi");
  EXPECT_EQ(u"hello world hi", str);
}

// ReplaceFirstSubstringAfterOffset tests

TEST(ReplaceFirstSubstringAfterOffsetTest, Basic) {
  std::string str = "hello world hello";
  ReplaceFirstSubstringAfterOffset(&str, 0, "hello", "hi");
  EXPECT_EQ("hi world hello", str);
}

TEST(ReplaceFirstSubstringAfterOffsetTest, WithOffset) {
  std::string str = "hello world hello";
  ReplaceFirstSubstringAfterOffset(&str, 6, "hello", "hi");
  EXPECT_EQ("hello world hi", str);
}

TEST(ReplaceFirstSubstringAfterOffsetTest, NoMatch) {
  std::string str = "hello world";
  ReplaceFirstSubstringAfterOffset(&str, 0, "xyz", "abc");
  EXPECT_EQ("hello world", str);
}

TEST(ReplaceFirstSubstringAfterOffsetTest, EmptyFind) {
  std::string str = "hello";
  ReplaceFirstSubstringAfterOffset(&str, 0, "", "xyz");
  EXPECT_EQ("hello", str);
}

TEST(ReplaceFirstSubstringAfterOffsetTest, ReplaceWithLonger) {
  std::string str = "a.b.c";
  ReplaceFirstSubstringAfterOffset(&str, 0, ".", "::");
  EXPECT_EQ("a::b.c", str);
}

// WriteInto tests

TEST(WriteIntoTest, StdString) {
  std::string str;
  char* data = WriteInto(&str, 6);
  ASSERT_NE(nullptr, data);
  EXPECT_EQ(5u, str.size());
  std::memcpy(data, "hello", 5);
  EXPECT_EQ("hello", str);
}

TEST(WriteIntoTest, StdWstring) {
  std::wstring str;
  wchar_t* data = WriteInto(&str, 4);
  ASSERT_NE(nullptr, data);
  EXPECT_EQ(3u, str.size());
  data[0] = L'a';
  data[1] = L'b';
  data[2] = L'c';
  EXPECT_EQ(L"abc", str);
}
