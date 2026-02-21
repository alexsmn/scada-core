#include "scada/date_time.h"

#include "base/format_time.h"
#include "base/utf_convert.h"

std::string ToString(scada::DateTime time) {
  return FormatTime(time);
}

std::u16string ToString16(scada::DateTime time) {
  return UtfConvert<char16_t>(FormatTime(time));
}

/*std::ostream& operator<<(std::ostream& stream, scada::DateTime time) {
  return stream << FormatTime(time);
}*/
