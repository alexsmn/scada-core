#include "scada/variant_utils.h"

#include "base/utf_convert.h"

namespace scada {

template <>
bool ConvertVariant(const Variant& source, std::wstring& target) {
  scada::LocalizedText localized_text;
  if (!ConvertVariant(source, localized_text))
    return false;
  target = UtfConvert<wchar_t>(localized_text);
  return true;
}

}  // namespace scada
