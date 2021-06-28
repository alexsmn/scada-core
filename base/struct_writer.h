#pragma once

#include "base/debug_util.h"
#include "base/strings/sys_string_conversions.h"

#include <ostream>
#include <string_view>

// Usage:
// std::ostream& operator<<(std::ostream& stream, const MyType& x) {
//   constexpr std::string_view kVerbStrings[] = {"Bit0", "Bit1", "Bit02"};
//   StructWriter{stream}
//       .AddField("a", x.a)
//       .AddBitMaskField("b", x.b, kEnumStrings)
//       .AddField("c", x.c);
//   return stream;
// }
class StructWriter {
 public:
  explicit StructWriter(std::ostream& stream) : stream_{stream} {
    stream_ << "{";
  }

  ~StructWriter() { stream_ << "}"; }

  template <class T>
  StructWriter& AddField(std::string_view name, const T& value) {
    if (count_ != 0)
      stream_ << ", ";
    stream_ << name << ": ";
    AddValue(value);
    ++count_;
    return *this;
  }

  StructWriter& AddBitMaskField(
      std::string_view name,
      unsigned bit_mask,
      base::span<const std::string_view> bit_strings) {
    if (count_ != 0)
      stream_ << ", ";
    stream_ << name << ": " << BitMaskToString(bit_mask, bit_strings);
    ++count_;
    return *this;
  }

 private:
  template <class T>
  void AddValue(const T& value) {
    stream_ << value;
  }

  template <>
  void AddValue(const char* const& value) {
    stream_ << "\"" << value << "\"";
  }

  template <>
  void AddValue(const std::string& value) {
    stream_ << "\"" << value << "\"";
  }

  template <>
  void AddValue(const std::wstring& value) {
    stream_ << "\"" << base::SysWideToNativeMB(value) << "\"";
  }

  std::ostream& stream_;
  std::size_t count_ = 0;
};
