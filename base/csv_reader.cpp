#include "base/csv_reader.h"

#include "base/string_util.h"
#include "base/utf_convert.h"

CsvReader::CsvReader(std::istream& stream, std::u16string_view signature)
    : stream_{stream}, signature_{signature} {}

bool CsvReader::ReadPhysicalLine() {
  if (!std::getline(stream_, raw_line_))
    return false;

  line_ = UtfConvert<char16_t>(raw_line_);
  if (!line_.empty() && line_.back() == u'\r') {
    line_.pop_back();
  }

  // Normalize EOL sequences so that we uniformly use a single LF character.
  ReplaceSubstringsAfterOffset(&line_, 0, u"\r\n", u"\n");
  line_pos_ = 0;
  return true;
}

bool CsvReader::NextRow() {
  ++row_index_;
  cell_index_ = 0;
  line_pos_ = 0;
  if (!ReadPhysicalLine())
    return false;

  has_cells_ = true;

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
    for (;;) {
      auto p = line_.find(u'"', line_pos_);
      if (p == std::u16string::npos) {
        // RFC 4180 §2 rule 6
        // (https://www.rfc-editor.org/rfc/rfc4180#section-2): a quoted field
        // may span line breaks, which is how both CsvWriter and Excel store a
        // cell holding a newline. The record continues on the next physical
        // line; only end of input leaves the cell unterminated.
        str += line_.substr(line_pos_);
        if (!ReadPhysicalLine()) {
          has_cells_ = false;
          return false;
        }
        str += u'\n';
        continue;
      }
      str += line_.substr(line_pos_, p - line_pos_);
      line_pos_ = p + 1;  // skip quote
      if (line_pos_ >= line_.size() || line_[line_pos_] != u'"')
        break;
      str += u'"';  // RFC 4180 §2 rule 7: "" is a literal quote.
      ++line_pos_;
    }

    // A well-formed quoted cell ends at a separator or at the end of the line.
    // Anything else between the closing quote and the separator is malformed
    // external input rather than an invariant of ours, so it must not
    // fail-stop; keep it as part of the cell, which is what Excel does with
    // `"abc"def`.
    auto sep = line_.find(separator_, line_pos_);
    if (sep == std::u16string::npos) {
      str += line_.substr(line_pos_);
      line_pos_ = line_.size();
      // The quoted cell was the last one on the line. Leaving `has_cells_` set
      // here used to report a phantom empty cell after it.
      has_cells_ = false;
    } else {
      str += line_.substr(line_pos_, sep - line_pos_);
      line_pos_ = sep + 1;
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
