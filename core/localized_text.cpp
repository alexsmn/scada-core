#include "core/localized_text.h"

#include "base/string_piece_util.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"

namespace scada {

LocalizedText ToLocalizedText(std::string_view string) {
  return base::WideToUTF16(base::SysNativeMBToWide(AsStringPiece(string)));
}

}  // namespace scada

std::string ToString(const scada::LocalizedText& text) {
  return base::SysWideToNativeMB(base::UTF16ToWide(text));
}
