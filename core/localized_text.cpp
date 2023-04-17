#include "core/localized_text.h"

#include "base/string_piece_util.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"

namespace scada {

LocalizedText ToLocalizedText(std::string_view string) {
  return base::UTF8ToUTF16(AsStringPiece(string));
}

}  // namespace scada

std::string ToString(const scada::LocalizedText& text) {
  // E.g. see `AuthenticationTask` on how login user name is translated to the
  // qualified name.
  return base::SysWideToNativeMB(base::UTF16ToWide(text));
  // return base::UTF16ToUTF8(text);
}
