#include "base/csv_writer.h"

#include "base/string_piece_util.h"
#include "base/strings/string_util.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"

namespace {

// chrome/src/components/password_manager/core/browser/import/csv_writer.cc
std::wstring StringToCsv(std::wstring_view raw_value,
                         wchar_t delimiter,
                         wchar_t quote) {
  std::wstring result;
  result.reserve(raw_value.size());
  // Fields containing line breaks (CRLF), double quotes, and commas should be
  // enclosed in double-quotes. If double-quotes are used to enclose fields,
  // then double-quotes appearing inside a field must be escaped by preceding
  // them with another double quote.
  const wchar_t escape[] = {L'\r', L'\n', delimiter, quote, L'\0'};
  if (raw_value.find_first_of(escape) != std::wstring::npos) {
    result.push_back(quote);
    result.append(raw_value.begin(), raw_value.end());
    const wchar_t two_quotes[] = {quote, quote, L'\0'};
    base::ReplaceSubstringsAfterOffset(
        &result, result.size() - raw_value.size(),
        base::StringPiece16{&quote, 1}, ToStringPiece(two_quotes));
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

void CsvWriter::WriteCell(std::wstring_view utf16) {
  if (!start_of_line_)
    stream_ << delimiter;
  start_of_line_ = false;

  auto escaped = StringToCsv(utf16, delimiter, quote);

#if defined(OS_WIN)
  std::string encoded =
      unicode ? base::UTF16ToUTF8(escaped) : base::SysWideToNativeMB(escaped);
#else
  std::string encoded = base::UTF16ToUTF8(escaped);
#endif

  stream_ << encoded;
  if (!stream_)
    throw std::runtime_error("Write error");
}
