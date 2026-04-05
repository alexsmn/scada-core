#pragma once

#include <istream>
#include <string_view>

class CsvReader {
 public:
  // |signature| is expected contents of the first cell useful to determine
  // separator.
  CsvReader(std::istream& stream, std::u16string_view signature = {});

  int row_index() const { return row_index_; }
  int cell_index() const { return cell_index_; }

  bool NextRow();
  bool NextCell(std::u16string& str);

 private:
  std::istream& stream_;
  std::u16string_view signature_;
  char16_t separator_ = u',';
  std::string raw_line_;
  std::u16string line_;
  std::u16string::size_type line_pos_ = 0;
  bool has_cells_ = false;
  int row_index_ = 0;
  int cell_index_ = 1;
};
