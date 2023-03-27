#include "base/struct_writer.h"

#include <boost/locale/encoding_utf.hpp>

template <>
void StructWriter::AddValue(const std::wstring& value) {
  stream_ << "\"" << boost::locale::conv::utf_to_utf<char>(value) << "\"";
}
