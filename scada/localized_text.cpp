#include "scada/localized_text.h"

#include "base/strings/sys_string_conversions.h"

#include "base/utf_convert.h"

namespace scada {

LocalizedText ToLocalizedText(std::string_view string) {
  return UtfConvert<char16_t>(string);
}

}  // namespace scada

std::string ToString(const scada::LocalizedText& text) {
  // E.g. see `AuthenticationTask` on how login user name is translated to the
  // qualified name.
  return base::SysWideToNativeMB(
      UtfConvert<wchar_t>(text));
}
