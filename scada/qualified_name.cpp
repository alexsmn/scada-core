#include "scada/qualified_name.h"

#include "base/strings/utf_string_conversions.h"

std::u16string ToString16(const scada::QualifiedName& name) {
  return base::UTF8ToUTF16(name.name());
}
