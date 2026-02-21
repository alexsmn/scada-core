#include "base/struct_writer.h"

#include "base/utf_convert.h"

template <>
void StructWriter::AddValue(const std::wstring& value) {
  stream_ << "\"" << UtfConvert<char>(value) << "\"";
}
