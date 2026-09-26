#pragma once

#include <istream>
#include <string>
#include <string_view>

// Reads RFC 4180 CSV record by record, cell by cell, decoding UTF-8 input to
// UTF-16 cells.
class CsvReader {
 public:
  // |signature| is expected contents of the first cell useful to determine
  // separator.
  CsvReader(std::istream& stream, std::u16string_view signature = {});

  int row_index() const { return row_index_; }
  int cell_index() const { return cell_index_; }

  // Advances to the next record. A record is one line, or several when a
  // quoted cell spans line breaks. Returns false at end of input.
  bool NextRow();

  // Reads the next cell of the current record into |str|. Returns false when
  // the record has no more cells, or when a quoted cell is still open at end
  // of input.
  bool NextCell(std::u16string& str);

 private:
  // Reads one physical line into |line_| and rewinds |line_pos_|.
  bool ReadPhysicalLine();

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
