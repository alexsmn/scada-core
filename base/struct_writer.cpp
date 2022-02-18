#include "base/struct_writer.h"

#include "base/strings/sys_string_conversions.h"

template <>
void StructWriter::AddValue(const std::wstring& value) {
  stream_ << "\"" << base::SysWideToNativeMB(value) << "\"";
}
