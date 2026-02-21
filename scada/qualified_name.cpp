#include "scada/qualified_name.h"

#include "base/utf_convert.h"

std::u16string ToString16(const scada::QualifiedName& name) {
  return UtfConvert<char16_t>(name.name());
}
