#include "core/qualified_name_util.h"

#include <boost/locale/encoding_utf.hpp>

namespace scada {

QualifiedName ToQualifiedName(std::u16string_view string) {
  auto u8string = boost::locale::conv::utf_to_utf<char>(
      string.data(), string.data() + string.size());
  return QualifiedName{u8string};
}

}  // namespace scada
