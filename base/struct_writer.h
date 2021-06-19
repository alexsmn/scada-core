#pragma once

#include "base/debug_util.h"
#include "base/strings/sys_string_conversions.h"

#include <ostream>
#include <string_view>

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
