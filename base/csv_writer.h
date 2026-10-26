#pragma once

#include <ostream>
#include <string>
#include <string_view>

// How a CsvWriter turns cell text into bytes.
struct CsvEncoding {
  // Converts one cell's text to the bytes written for it.
  using Encoder = std::string (*)(std::u16string_view utf16);

  // Written once, before anything else; empty for none.
  std::string bom;
  Encoder encode = nullptr;
};

// UTF-8 preceded by a byte-order mark. The mark is what lets Excel tell UTF-8
// from the ANSI code page; without it Excel opens the file as cp1251 on Russian
// Windows and garbles every Cyrillic cell.
CsvEncoding Utf8CsvEncoding();

// The platform's "system" encoding. On Windows it is the ANSI code page
// (cp1251 on Russian Windows) with no byte-order mark, which is what Excel
// assumes for a file without one; a character the code page cannot represent
// is written as '?'. A Windows whose ANSI code page is UTF-8 gets
// Utf8CsvEncoding(). Everywhere else the system encoding is UTF-8, so this is
// Utf8CsvEncoding() too: there is no legacy code page to fall back to, and
// Excel for Mac needs the mark as much as Excel for Windows does.
CsvEncoding SystemCsvEncoding();

// Writes RFC 4180 CSV: rows end "\r\n", so `stream` must be opened in binary
// mode, or Windows turns each row end into "\r\r\n".
class CsvWriter {
 public:
  explicit CsvWriter(std::ostream& stream);

  void StartRow();

  void WriteCell(std::string_view utf8);
  void WriteCell(std::u16string_view utf16);

  void SkipCell() { WriteCell(std::u16string_view{}); }

  // Set before the first StartRow() or WriteCell().
  CsvEncoding encoding = Utf8CsvEncoding();
  char delimiter = ',';
  char quote = '"';

 private:
  void WriteBom();

  std::ostream& stream_;
  bool bom_written_ = false;
  bool skip_start_ = true;
  bool start_of_line_ = true;
};
