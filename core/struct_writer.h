#pragma once

#include "core/debug_util.h"

#include <ostream>
#include <string_view>

class StructWriter {
 public:
  explicit StructWriter(std::ostream& stream) : stream_{stream} {}

  void BeginStruct() { stream_ << "{"; }
  void EndStruct() { stream_ << "}"; }

  template <class T>
  void AddField(std::string_view name, const T& value) {
    if (count_ != 0)
      stream_ << ", ";
    stream_ << name << ": ";
    AddValue(value);
    ++count_;
  }

  template <class T>
  void AddValue(const T& value) {
    stream_ << value;
  }

  template <>
  void AddValue(const std::string& value) {
    stream_ << "\"" << value << "\"";
  }

 private:
  std::ostream& stream_;
  int count_ = 0;
};
