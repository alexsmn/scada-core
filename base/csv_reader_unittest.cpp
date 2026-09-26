#include "base/csv_reader.h"

#include <gmock/gmock.h>
#include <sstream>
#include <string>
#include <vector>

using namespace testing;

namespace {

// Reads every record of |csv| as a list of cells.
std::vector<std::vector<std::u16string>> ReadAll(
    const std::string& csv,
    std::u16string_view signature = {}) {
  std::istringstream stream{csv};
  CsvReader reader{stream, signature};
  std::vector<std::vector<std::u16string>> rows;
  while (reader.NextRow()) {
    std::vector<std::u16string>& row = rows.emplace_back();
    std::u16string cell;
    while (reader.NextCell(cell))
      row.push_back(cell);
  }
  return rows;
}

TEST(CsvReader, PlainCells) {
  EXPECT_THAT(ReadAll("a,b\r\nc,d\r\n"),
              ElementsAre(ElementsAre(u"a", u"b"), ElementsAre(u"c", u"d")));
}

TEST(CsvReader, TrailingSeparatorIsAnEmptyCell) {
  EXPECT_THAT(ReadAll("a,\r\n"), ElementsAre(ElementsAre(u"a", u"")));
}

TEST(CsvReader, QuotedCellKeepsSeparatorAndEscapedQuote) {
  EXPECT_THAT(ReadAll("\"a,\"\"b\"\"\",c\r\n"),
              ElementsAre(ElementsAre(u"a,\"b\"", u"c")));
}

// Regression: a quoted cell ending the line left the reader believing another
// cell followed, so every such record gained a phantom empty cell. A header
// whose last column needed quoting failed the configuration import as
// "Invalid column name format".
TEST(CsvReader, QuotedLastCellHasNoPhantomCellAfterIt) {
  EXPECT_THAT(ReadAll("a,\"b,c\"\r\nd,e\r\n"),
              ElementsAre(ElementsAre(u"a", u"b,c"), ElementsAre(u"d", u"e")));
}

TEST(CsvReader, QuotedCellFollowedBySeparatorAtEndOfLine) {
  EXPECT_THAT(ReadAll("\"a\",\r\n"), ElementsAre(ElementsAre(u"a", u"")));
}

// Regression: RFC 4180 lets a quoted cell span line breaks, and CsvWriter and
// Excel both write a cell holding a newline that way. The reader split the
// record at the physical line and reported the cell as unterminated.
TEST(CsvReader, QuotedCellSpansLineBreaks) {
  EXPECT_THAT(
      ReadAll("a,\"two\r\nlines\",b\r\nc\r\n"),
      ElementsAre(ElementsAre(u"a", u"two\nlines", u"b"), ElementsAre(u"c")));
}

TEST(CsvReader, UnterminatedQuoteAtEndOfInputEndsTheRecord) {
  std::istringstream stream{"a,\"open\r\nstill open"};
  CsvReader reader{stream};
  ASSERT_TRUE(reader.NextRow());
  std::u16string cell;
  EXPECT_TRUE(reader.NextCell(cell));
  EXPECT_EQ(cell, u"a");
  EXPECT_FALSE(reader.NextCell(cell));
  EXPECT_FALSE(reader.NextCell(cell));
  EXPECT_FALSE(reader.NextRow());
}

// Regression: text between a closing quote and the separator tripped a
// base::Check, fail-stopping the client on a file the operator chose. It is
// malformed input, and is kept as part of the cell the way Excel reads it.
TEST(CsvReader, TextAfterClosingQuoteIsKeptNotFatal) {
  EXPECT_THAT(ReadAll("\"abc\"def,g\r\n\"x\"y\r\n"),
              ElementsAre(ElementsAre(u"abcdef", u"g"), ElementsAre(u"xy")));
}

TEST(CsvReader, SignatureSelectsSeparator) {
  EXPECT_THAT(
      ReadAll("Id;Name\r\n1;\"a;b\"\r\n", u"Id"),
      ElementsAre(ElementsAre(u"Id", u"Name"), ElementsAre(u"1", u"a;b")));
}

}  // namespace
