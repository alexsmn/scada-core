#include "base/format.h"

#include "base/string_piece_util.h"

#include <base/strings/string_number_conversions.h>
#include <base/strings/string_util.h>
#include <base/third_party/dmg_fp/dmg_fp.h>
#include <boost/locale/encoding_utf.hpp>
#include <memory>

std::string Format(double value) {
  char buffer[32];
  dmg_fp::g_fmt(buffer, value);
  return std::string(buffer);
}

std::u16string WideFormat(double value) {
  char buffer[32];
  dmg_fp::g_fmt(buffer, value);
  return std::u16string(buffer, buffer + strlen(buffer));
}

std::string Format(float value) {
  return Format(static_cast<double>(value));
}

std::u16string WideFormat(float value) {
  return WideFormat(static_cast<double>(value));
}

std::string Format(int value) {
  return base::NumberToString(value);
}

std::u16string WideFormat(int value) {
  return base::NumberToString16(value);
}

std::string Format(unsigned int value) {
  return base::NumberToString(value);
}

std::u16string WideFormat(unsigned int value) {
  return base::NumberToString16(value);
}

std::string Format(long value) {
  return base::NumberToString(value);
}

std::u16string WideFormat(long value) {
  return base::NumberToString16(value);
}

std::string Format(unsigned long value) {
  return base::NumberToString(value);
}

std::u16string WideFormat(unsigned long value) {
  return base::NumberToString16(value);
}

std::string Format(long long value) {
  return base::NumberToString(value);
}

std::u16string WideFormat(long long value) {
  return base::NumberToString16(value);
}

std::string Format(unsigned long long value) {
  return base::NumberToString(value);
}

std::u16string WideFormat(unsigned long long value) {
  return base::NumberToString16(value);
}

// String can be implicitly casted to bool.
/*std::string Format(bool value) {
  return Format(value ? 1 : 0);
}

std::u16string WideFormat(bool value) {
  return WideFormat(value ? 1 : 0);
}*/

std::string Format(unsigned char value) {
  return Format(static_cast<unsigned>(value));
}

std::u16string WideFormat(unsigned char value) {
  return WideFormat(static_cast<unsigned>(value));
}

std::string Format(unsigned short value) {
  return Format(static_cast<unsigned>(value));
}

std::u16string WideFormat(unsigned short value) {
  return WideFormat(static_cast<unsigned>(value));
}

std::string Format(std::string_view value) {
  return std::string{value};
}

std::u16string WideFormat(std::string_view value) {
  return boost::locale::conv::utf_to_utf<char16_t>(value.data(),
                                                   value.data() + value.size());
}

std::string Format(std::u16string_view value) {
  return boost::locale::conv::utf_to_utf<char>(value.data(),
                                               value.data() + value.size());
}

std::u16string WideFormat(std::u16string_view value) {
  return std::u16string{value};
}

template <>
bool Parse(const std::string_view& str, int32_t& value) {
  return base::StringToInt(AsStringPiece(str), &value);
}

template <>
bool Parse(const std::u16string_view& str, int32_t& value) {
  return base::StringToInt(AsStringPiece(str), &value);
}

template <>
bool Parse(const std::string_view& str, uint32_t& value) {
  return base::StringToUint(AsStringPiece(str), &value);
}

template <>
bool Parse(const std::u16string_view& str, uint32_t& value) {
  return base::StringToUint(AsStringPiece(str), &value);
}

template <>
bool Parse(const std::string_view& str, int8_t& value) {
  int tmp;
  if (!Parse(str, tmp))
    return false;
  value = static_cast<int8_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::u16string_view& str, int8_t& value) {
  int tmp;
  if (!Parse(str, tmp))
    return false;
  value = static_cast<int8_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::string_view& str, uint8_t& value) {
  unsigned tmp;
  if (!Parse(str, tmp))
    return false;
  value = static_cast<uint8_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::u16string_view& str, uint8_t& value) {
  unsigned tmp;
  if (!Parse(str, tmp))
    return false;
  value = static_cast<uint8_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::string_view& str, int16_t& value) {
  int tmp;
  if (!base::StringToInt(AsStringPiece(str), &tmp))
    return false;
  value = static_cast<int16_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::u16string_view& str, int16_t& value) {
  int tmp;
  if (!base::StringToInt(AsStringPiece(str), &tmp))
    return false;
  value = static_cast<int16_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::string_view& str, uint16_t& value) {
  unsigned tmp;
  if (!base::StringToUint(AsStringPiece(str), &tmp))
    return false;
  value = static_cast<uint16_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::u16string_view& str, uint16_t& value) {
  unsigned tmp;
  if (!base::StringToUint(AsStringPiece(str), &tmp))
    return false;
  value = static_cast<unsigned short>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::string_view& str, int64_t& value) {
  return base::StringToInt64(AsStringPiece(str), &value);
}

template <>
bool Parse(const std::u16string_view& str, int64_t& value) {
  return base::StringToInt64(AsStringPiece(str), &value);
}

template <>
bool Parse(const std::string_view& str, uint64_t& value) {
  return base::StringToUint64(AsStringPiece(str), &value);
}

template <>
bool Parse(const std::u16string_view& str, uint64_t& value) {
  return base::StringToUint64(AsStringPiece(str), &value);
}

template <>
bool Parse(const std::string_view& str, double& value) {
  return base::StringToDouble(AsStringPiece(str), &value);
}
template <>
bool Parse(const std::u16string_view& str, double& value) {
  return base::StringToDouble(AsStringPiece(str), &value);
}

template <>
bool Parse(const std::string_view& str, bool& value) {
  int tmp;
  if (!Parse(str, tmp))
    return false;
  value = tmp != 0;
  return true;
}

template <>
bool Parse(const std::u16string_view& str, bool& value) {
  int tmp;
  if (!Parse(str, tmp))
    return false;
  value = tmp != 0;
  return true;
}

template <>
bool Parse(const std::string_view& str, std::string& value) {
  value = std::string{str};
  return true;
}
template <>
bool Parse(const std::u16string_view& str, std::u16string& value) {
  value = std::u16string{str};
  return true;
}

std::string FormatHexBuffer(const void* buf, size_t len) {
  assert(buf);
  if (!len)
    return std::string();

  return base::HexEncode(buf, len);
}
