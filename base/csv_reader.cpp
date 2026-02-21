#include "base/csv_reader.h"

#include "base/string_util.h"
#include "base/utf_convert.h"
#include <cassert>

CsvReader::CsvReader(std::istream& stream, std::u16string_view signature)
    : stream_{stream}, signature_{signature} {}

bool CsvReader::NextRow() {
  ++row_index_;
  cell_index_ = 0;
  line_pos_ = 0;
  if (!std::getline(stream_, raw_line_))
    return false;

  line_ = UtfConvert<char16_t>(raw_line_);
  has_cells_ = true;

  // Normalize EOL sequences so that we uniformly use a single LF character.
  ReplaceSubstringsAfterOffset(&line_, 0, u"\r\n", u"\n");

  if (!signature_.empty()) {
    if (line_.size() > signature_.size() && line_.starts_with(signature_)) {
      separator_ = line_[signature_.size()];
    }
    signature_ = {};
  }
  return true;
}

bool CsvReader::NextCell(std::u16string& str) {
  str.clear();

  if (!has_cells_)
    return false;

  ++cell_index_;

  // Escaped.
  if (line_pos_ < line_.size() && line_[line_pos_] == u'"') {
    ++line_pos_;
    while (line_pos_ < line_.size()) {
      auto p = line_.find(u'"', line_pos_);
      if (p == std::string::npos) {
        assert(false);
        return false;
      }
      str += line_.substr(line_pos_, p - line_pos_);
      line_pos_ = p + 1;  // skip quote
      if (line_pos_ >= line_.size() || line_[line_pos_] != u'"')
        break;
      str += u'"';
      ++line_pos_;
    }
    // Should end with line break or separator.
    if (line_pos_ < line_.size()) {
      assert(line_[line_pos_] == separator_);
      ++line_pos_;
    }
    return true;
  }

  // Unescaped.
  auto p = line_.find_first_of(separator_, line_pos_);
  if (p == std::string::npos) {
    str = line_.substr(line_pos_);
    has_cells_ = false;
    return true;

  } else {
    str = line_.substr(line_pos_, p - line_pos_);
    line_pos_ = p + 1;
    return true;
  }
}
