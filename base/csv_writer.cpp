#include "base/csv_writer.h"

#include "base/string_util.h"
#include "base/utf_convert.h"

#if defined(_WIN32)
#include <windows.h>
#endif

#include <stdexcept>

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
    ReplaceSubstringsAfterOffset(&result, result.size() - raw_value.size(),
                                 std::u16string_view{&quote, 1}, two_quotes);
    result.push_back(quote);
  } else {
    result.append(raw_value.begin(), raw_value.end());
  }
  return result;
}

std::string EncodeUtf8(std::u16string_view utf16) {
  return UtfConvert<char>(utf16);
}

#if defined(_WIN32)
// Converts to the ANSI code page. A character the code page lacks becomes the
// system default character ('?'), which is what Excel would show for it anyway.
std::string EncodeAnsiCodePage(std::u16string_view utf16) {
  if (utf16.empty())
    return {};

  const wchar_t* wide = reinterpret_cast<const wchar_t*>(utf16.data());
  const int wide_size = static_cast<int>(utf16.size());
  const int size = ::WideCharToMultiByte(CP_ACP, 0, wide, wide_size, nullptr, 0,
                                         nullptr, nullptr);
  if (size <= 0)
    throw std::runtime_error("Encoding error");

  std::string result(static_cast<size_t>(size), '\0');
  ::WideCharToMultiByte(CP_ACP, 0, wide, wide_size, result.data(), size,
                        nullptr, nullptr);
  return result;
}
#endif

}  // namespace

CsvEncoding Utf8CsvEncoding() {
  return {.bom = "\xEF\xBB\xBF", .encode = &EncodeUtf8};
}

CsvEncoding SystemCsvEncoding() {
#if defined(_WIN32)
  if (::GetACP() != CP_UTF8)
    return {.encode = &EncodeAnsiCodePage};
#endif
  return Utf8CsvEncoding();
}

CsvWriter::CsvWriter(std::ostream& stream) : stream_{stream} {}

void CsvWriter::WriteBom() {
  if (bom_written_)
    return;
  bom_written_ = true;
  stream_ << encoding.bom;
}

void CsvWriter::StartRow() {
  auto skip = skip_start_;
  skip_start_ = false;
  WriteBom();
  if (skip)
    return;

  stream_ << "\r\n";
  if (!stream_)
    throw std::runtime_error("Write error");

  start_of_line_ = true;
}

void CsvWriter::WriteCell(std::string_view utf8) {
  WriteCell(UtfConvert<char16_t>(utf8));
}

void CsvWriter::WriteCell(std::u16string_view utf16) {
  WriteBom();
  if (!start_of_line_)
    stream_ << delimiter;
  start_of_line_ = false;

  auto escaped = StringToCsv(utf16, delimiter, quote);

  std::string encoded = encoding.encode(escaped);

  stream_ << encoded;
  if (!stream_)
    throw std::runtime_error("Write error");
}
