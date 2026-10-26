#include "base/csv_writer.h"

#include <gmock/gmock.h>
#include <sstream>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#endif

using namespace testing;

namespace {

const std::string kUtf8Bom = "\xEF\xBB\xBF";

// A stand-in for Windows-1251 that covers ASCII and the Cyrillic block
// А..я (U+0410..U+044F, which cp1251 maps to 0xC0..0xFF in order), so the
// "system" path can be exercised on a host that has no ANSI code page.
std::string EncodeFakeCp1251(std::u16string_view utf16) {
  std::string result;
  for (char16_t ch : utf16) {
    if (ch < 0x80)
      result.push_back(static_cast<char>(ch));
    else if (ch >= 0x0410 && ch <= 0x044F)
      result.push_back(static_cast<char>(0xC0 + (ch - 0x0410)));
    else
      result.push_back('?');
  }
  return result;
}

// Writes `rows` with `encoding` and returns the bytes.
std::string Write(
    const CsvEncoding& encoding,
    std::initializer_list<std::initializer_list<std::u16string>> rows) {
  std::ostringstream stream{std::ios::binary};
  CsvWriter writer{stream};
  writer.encoding = encoding;
  for (const auto& row : rows) {
    writer.StartRow();
    for (const auto& cell : row)
      writer.WriteCell(cell);
  }
  return stream.str();
}

TEST(CsvWriter, RowsEndCrLf) {
  EXPECT_EQ(
      Write({.encode = Utf8CsvEncoding().encode}, {{u"a", u"b"}, {u"c", u"d"}}),
      "a,b\r\nc,d");
}

TEST(CsvWriter, Utf8WritesBomOnceBeforeTheFirstCell) {
  EXPECT_EQ(Write(Utf8CsvEncoding(), {{u"Имя"}, {u"значение"}}),
            kUtf8Bom +
                "\xD0\x98\xD0\xBC\xD1\x8F\r\n"
                "\xD0\xB7\xD0\xBD\xD0\xB0\xD1\x87\xD0\xB5\xD0\xBD\xD0"
                "\xB8\xD0\xB5");
}

TEST(CsvWriter, BomIsWrittenEvenForAnEmptyHeaderRow) {
  std::ostringstream stream{std::ios::binary};
  CsvWriter writer{stream};
  writer.StartRow();
  EXPECT_EQ(stream.str(), kUtf8Bom);
}

TEST(CsvWriter, CodePageEncodingConvertsCellsAndWritesNoBom) {
  EXPECT_EQ(Write({.encode = &EncodeFakeCp1251}, {{u"Имя", u"x"}}),
            "\xC8\xEC\xFF,x");
}

TEST(CsvWriter, EscapingHappensBeforeEncoding) {
  EXPECT_EQ(Write({.encode = &EncodeFakeCp1251}, {{u"а,б", u"\"я\""}}),
            "\"\xE0,\xE1\",\"\"\"\xFF\"\"\"");
}

#if defined(_WIN32)
// The real conversion, on the one code page whose bytes this test knows.
TEST(CsvWriter, SystemEncodingIsCp1251OnRussianWindows) {
  if (::GetACP() != 1251)
    GTEST_SKIP() << "ANSI code page is " << ::GetACP() << ", not 1251";
  EXPECT_EQ(Write(SystemCsvEncoding(), {{u"Имя"}}), "\xC8\xEC\xFF");
}
#else
// Off Windows the system encoding is UTF-8, and it carries the BOM for the
// same reason the explicit UTF-8 choice does: Excel needs it to detect UTF-8.
TEST(CsvWriter, SystemEncodingIsUtf8WithBomOffWindows) {
  EXPECT_EQ(Write(SystemCsvEncoding(), {{u"Имя"}}),
            kUtf8Bom + "\xD0\x98\xD0\xBC\xD1\x8F");
}
#endif

}  // namespace
