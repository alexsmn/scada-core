#include "scada/qualified_name_util.h"

#include "base/utf_convert.h"

namespace scada {

QualifiedName ToQualifiedName(std::u16string_view string) {
  auto u8string = UtfConvert<char>(string);
  return QualifiedName{u8string};
}

}  // namespace scada
