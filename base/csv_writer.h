#pragma once

#include <ostream>
#include <string_view>

class CsvWriter {
 public:
  explicit CsvWriter(std::ostream& stream);

  void StartRow();

  void WriteCell(std::string_view utf8);
  void WriteCell(std::u16string_view utf16);

  void SkipCell() { WriteCell(std::u16string_view{}); }

  bool unicode = false;
  char delimiter = ',';
  char quote = '"';

 private:
  std::ostream& stream_;
  bool skip_start_ = true;
  bool start_of_line_ = true;
};
