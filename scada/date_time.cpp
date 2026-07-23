#include "scada/date_time.h"

#include "base/format_time.h"
#include "base/utf_convert.h"

std::string ToString(scada::Time time) {
  return FormatTime(time);
}

std::u16string ToString16(scada::Time time) {
  return UtfConvert<char16_t>(FormatTime(time));
}

/*std::ostream& operator<<(std::ostream& stream, scada::Time time) {
  return stream << FormatTime(time);
}*/
