#pragma once

#include "base/string_piece_util.h"
#include "base/values.h"

template <class T>
inline bool ReadJson(const base::Value& json, T& value);

template <>
inline bool ReadJson(const base::Value& json, bool& value) {
  return json.GetAsBoolean(&value);
}

template <>
inline bool ReadJson(const base::Value& json, std::string& value) {
  return json.GetAsString(&value);
}

template <>
inline bool ReadJson(const base::Value& json, std::u16string& value) {
  return json.GetAsString(&value);
}

template <class T>
class JsonReader {
 public:
  explicit JsonReader(const base::Value& json) : json_{json} {}

  template <class F>
  JsonReader& field(std::string_view key, F T::*field) {
    auto* json_value = json_.FindKey(AsStringPiece(key));
    if (!json_value)
      return *this;

    if (!ReadJson(*json_value, value_.*field))
      throw std::runtime_error{"Wrong JSON value"};

    return *this;
  }

  T Read() { return std::move(value_); }

 private:
  const base::Value& json_;
  T value_;
};
