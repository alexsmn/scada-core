#include "base/csv_writer.h"

#include "base/string_piece_util.h"

#include <base/strings/string_util.h>
#include <boost/locale/encoding_utf.hpp>

namespace {

// chrome/src/components/password_manager/core/browser/import/csv_writer.cc
std::u16string StringToCsv(std::u16string_view raw_value,
                           char16_t delimiter,
                           char16_t quote) {
  std::u16string result;
  result.reserve(raw_value.size());
  // Fields containing line breaks (CRLF), double quotes, and commas should be
  // enclosed in double-quotes. If double-quotes are used to enclose fields,
  // then double-quotes appearing inside a field must be escaped by preceding
  // them with another double quote.
  const char16_t escape[] = {u'\r', u'\n', delimiter, quote, u'\0'};
  if (raw_value.find_first_of(escape) != std::u16string::npos) {
    result.push_back(quote);
    result.append(raw_value.begin(), raw_value.end());
    const char16_t two_quotes[] = {quote, quote, u'\0'};
    base::ReplaceSubstringsAfterOffset(
        &result, result.size() - raw_value.size(),
        base::StringPiece16{&quote, 1}, two_quotes);
    result.push_back(quote);
  } else {
    result.append(raw_value.begin(), raw_value.end());
  }
  return result;
}

}  // namespace

CsvWriter::CsvWriter(std::ostream& stream) : stream_{stream} {}

void CsvWriter::StartRow() {
  auto skip = skip_start_;
  skip_start_ = false;
  if (skip)
    return;

  stream_ << std::endl;
  if (!stream_)
    throw std::runtime_error("Write error");

  start_of_line_ = true;
}

void CsvWriter::WriteCell(std::string_view utf8) {
  WriteCell(boost::locale::conv::utf_to_utf<char16_t>(
      utf8.data(), utf8.data() + utf8.size()));
}

void CsvWriter::WriteCell(std::u16string_view utf16) {
  if (!start_of_line_)
    stream_ << delimiter;
  start_of_line_ = false;

  auto escaped = StringToCsv(utf16, delimiter, quote);

  std::string encoded = boost::locale::conv::utf_to_utf<char>(escaped);

  stream_ << encoded;
  if (!stream_)
    throw std::runtime_error("Write error");
}
