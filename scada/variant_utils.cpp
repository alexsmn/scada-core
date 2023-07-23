#include "scada/variant_utils.h"

#include "base/strings/utf_string_conversions.h"

namespace scada {

template <>
bool ConvertVariant(const Variant& source, std::wstring& target) {
  scada::LocalizedText localized_text;
  if (!ConvertVariant(source, localized_text))
    return false;
  target = base::UTF16ToWide(localized_text);
  return true;
}

}  // namespace scada
