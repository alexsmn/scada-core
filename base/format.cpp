#include "base/format.h"

#include "base/string_piece_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/third_party/dmg_fp/dmg_fp.h"

#include <memory>

std::string Format(double value) {
  char buffer[32];
  dmg_fp::g_fmt(buffer, value);
  return std::string(buffer);
}

std::wstring WideFormat(double value) {
  char buffer[32];
  dmg_fp::g_fmt(buffer, value);
  return std::wstring(buffer, buffer + strlen(buffer));
}

std::string Format(float value) {
  return Format(static_cast<double>(value));
}

std::wstring WideFormat(float value) {
  return WideFormat(static_cast<double>(value));
}

std::string Format(int value) {
  return base::NumberToString(value);
}

std::wstring WideFormat(int value) {
  return base::NumberToString16(value);
}

std::string Format(unsigned int value) {
  return base::NumberToString(value);
}

std::wstring WideFormat(unsigned int value) {
  return base::NumberToString16(value);
}

std::string Format(uint64_t value) {
  return base::NumberToString(value);
}

std::string Format(int64_t value) {
  return base::NumberToString(value);
}

std::wstring WideFormat(int64_t value) {
  return base::NumberToString16(value);
}

std::wstring WideFormat(uint64_t value) {
  return base::NumberToString16(value);
}

std::string Format(bool value) {
  return Format(value ? 1 : 0);
}

std::wstring WideFormat(bool value) {
  return WideFormat(value ? 1 : 0);
}

std::string Format(unsigned char value) {
  return Format(static_cast<unsigned>(value));
}

std::wstring WideFormat(unsigned char value) {
  return WideFormat(static_cast<unsigned>(value));
}

std::string Format(unsigned short value) {
  return Format(static_cast<unsigned>(value));
}

std::wstring WideFormat(unsigned short value) {
  return WideFormat(static_cast<unsigned>(value));
}

std::string Format(std::string_view value) {
  return std::string{value};
}

std::wstring WideFormat(std::string_view value) {
  return base::WideToUTF16(base::SysNativeMBToWide(ToStringPiece(value)));
}

std::string Format(std::wstring_view value) {
  return base::SysWideToNativeMB(base::UTF16ToWide(ToStringPiece(value)));
}

std::wstring WideFormat(std::wstring_view value) {
  return std::wstring{value};
}

template <>
bool Parse(const std::string_view& str, int32_t& value) {
  return base::StringToInt(ToStringPiece(str), &value);
}

template <>
bool Parse(const std::wstring_view& str, int32_t& value) {
  return base::StringToInt(ToStringPiece(str), &value);
}

template <>
bool Parse(const std::string_view& str, uint32_t& value) {
  return base::StringToUint(ToStringPiece(str), &value);
}

template <>
bool Parse(const std::wstring_view& str, uint32_t& value) {
  return base::StringToUint(ToStringPiece(str), &value);
}

template <>
bool Parse(const std::string_view& str, uint8_t& value) {
  unsigned tmp;
  if (!Parse(str, tmp))
    return false;
  value = static_cast<unsigned char>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::wstring_view& str, uint8_t& value) {
  unsigned tmp;
  if (!Parse(str, tmp))
    return false;
  value = static_cast<uint8_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::string_view& str, int16_t& value) {
  int tmp;
  if (!base::StringToInt(ToStringPiece(str), &tmp))
    return false;
  value = static_cast<int16_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::wstring_view& str, int16_t& value) {
  int tmp;
  if (!base::StringToInt(ToStringPiece(str), &tmp))
    return false;
  value = static_cast<int16_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::string_view& str, uint16_t& value) {
  unsigned tmp;
  if (!base::StringToUint(ToStringPiece(str), &tmp))
    return false;
  value = static_cast<uint16_t>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::wstring_view& str, uint16_t& value) {
  unsigned tmp;
  if (!base::StringToUint(ToStringPiece(str), &tmp))
    return false;
  value = static_cast<unsigned short>(tmp);
  return value == tmp;
}

template <>
bool Parse(const std::string_view& str, int64_t& value) {
  return base::StringToInt64(ToStringPiece(str), &value);
}

template <>
bool Parse(const std::wstring_view& str, int64_t& value) {
  return base::StringToInt64(ToStringPiece(str), &value);
}

template <>
bool Parse(const std::string_view& str, uint64_t& value) {
  return base::StringToUint64(ToStringPiece(str), &value);
}

template <>
bool Parse(const std::wstring_view& str, uint64_t& value) {
  return base::StringToUint64(ToStringPiece(str), &value);
}

template <>
bool Parse(const std::string_view& str, double& value) {
  return base::StringToDouble(std::string{str}, &value);
}
template <>
bool Parse(const std::wstring_view& str, double& value) {
  // TODO: Use ICU for conversion.
  return base::StringToDouble(base::UTF16ToASCII(ToStringPiece(str)), &value);
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
bool Parse(const std::wstring_view& str, bool& value) {
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
bool Parse(const std::wstring_view& str, std::wstring& value) {
  value = std::wstring{str};
  return true;
}

std::string FormatHexBuffer(const void* buf, size_t len) {
  assert(buf);
  if (!len)
    return std::string();

  return base::HexEncode(buf, len);
}
