#pragma once

#include "base/utf_convert.h"

#include <boost/json.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

template <class T>
inline bool ReadJson(const boost::json::value& json, T& value);

template <>
inline bool ReadJson(const boost::json::value& json, bool& value) {
  if (!json.is_bool())
    return false;
  value = json.as_bool();
  return true;
}

template <>
inline bool ReadJson(const boost::json::value& json, std::string& value) {
  if (!json.is_string())
    return false;
  value = std::string{json.as_string()};
  return true;
}

template <>
inline bool ReadJson(const boost::json::value& json, std::u16string& value) {
  if (!json.is_string())
    return false;
  value = UtfConvert<char16_t>(std::string_view{json.as_string()});
  return true;
}

template <class T>
class JsonReader {
 public:
  explicit JsonReader(const boost::json::value& json) : json_{json} {}

  template <class F>
  JsonReader& field(std::string_view key, F T::*field) {
    if (!json_.is_object())
      return *this;
    auto* json_value = json_.as_object().if_contains(key);
    if (!json_value)
      return *this;

    if (!ReadJson(*json_value, value_.*field)) {
      throw std::runtime_error{"Wrong JSON value"};
    }

    return *this;
  }

  T Read() { return std::move(value_); }

 private:
  const boost::json::value& json_;
  T value_;
};
