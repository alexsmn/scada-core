#include "core/qualified_name.h"

#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"

std::wstring ToString16(const scada::QualifiedName& name) {
  return base::WideToUTF16(base::SysNativeMBToWide(name.name()));
}
