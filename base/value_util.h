#pragma once

#include "base/utf_convert.h"

#include <boost/json.hpp>
#include <optional>
#include <string>
#include <string_view>

inline bool GetBool(const boost::json::value& value,
                    std::string_view key,
                    bool default_value = false) {
  if (!value.is_object())
    return default_value;
  auto* k = value.as_object().if_contains(key);
  if (!k || !k->is_bool())
    return default_value;
  return k->as_bool();
}

inline int GetInt(const boost::json::value& value,
                  std::string_view key,
                  int default_value = 0) {
  if (!value.is_object())
    return default_value;
  auto* k = value.as_object().if_contains(key);
  if (!k || !k->is_int64())
    return default_value;
  return static_cast<int>(k->as_int64());
}

inline std::string_view GetString(const boost::json::value& value,
                                  std::string_view key,
                                  std::string_view default_value = {}) {
  if (!value.is_object())
    return default_value;
  auto* k = value.as_object().if_contains(key);
  if (!k || !k->is_string())
    return default_value;
  return k->as_string();
}

inline std::u16string GetString16(const boost::json::value& value,
                                  std::string_view key,
                                  std::u16string_view default_value = {}) {
  if (!value.is_object())
    return std::u16string{default_value};
  auto* k = value.as_object().if_contains(key);
  if (!k || !k->is_string())
    return std::u16string{default_value};
  return UtfConvert<char16_t>(std::string_view{k->as_string()});
}

inline const boost::json::value* FindDict(const boost::json::value& value,
                                          std::string_view key) {
  if (!value.is_object())
    return nullptr;
  auto* k = value.as_object().if_contains(key);
  if (!k || !k->is_object())
    return nullptr;
  return k;
}

inline const boost::json::value& GetDict(const boost::json::value& value,
                                         std::string_view key) {
  static const boost::json::value kEmptyObject =
      boost::json::value(boost::json::object{});
  const auto* dict = FindDict(value, key);
  return dict ? *dict : kEmptyObject;
}

inline const boost::json::array* GetList(const boost::json::value& value,
                                         std::string_view key) {
  if (!value.is_object())
    return nullptr;
  auto* k = value.as_object().if_contains(key);
  if (!k || !k->is_array())
    return nullptr;
  return &k->as_array();
}

template <class T>
inline std::optional<T> GetKey(const boost::json::value& dict,
                               std::string_view key);

template <>
inline std::optional<int> GetKey(const boost::json::value& dict,
                                 std::string_view key) {
  if (!dict.is_object())
    return std::nullopt;
  auto* k = dict.as_object().if_contains(key);
  if (!k || !k->is_int64())
    return std::nullopt;
  return static_cast<int>(k->as_int64());
}

inline const boost::json::value& GetKey(const boost::json::value& dict,
                                        std::string_view key) {
  static const boost::json::value kNull;
  if (!dict.is_object())
    return kNull;
  auto* k = dict.as_object().if_contains(key);
  return k ? *k : kNull;
}

inline void SetKey(boost::json::value& dict,
                   std::string_view key,
                   bool value) {
  dict.as_object()[key] = value;
}

inline void SetKey(boost::json::value& dict,
                   std::string_view key,
                   int value) {
  dict.as_object()[key] = value;
}

inline void SetKey(boost::json::value& dict,
                   std::string_view key,
                   const char* value) {
  dict.as_object()[key] = value;
}

inline void SetKey(boost::json::value& dict,
                   std::string_view key,
                   const char16_t* value) {
  dict.as_object()[key] = UtfConvert<char>(std::u16string_view{value});
}

inline void SetKey(boost::json::value& dict,
                   std::string_view key,
                   std::string_view value) {
  dict.as_object()[key] = std::string{value};
}

inline void SetKey(boost::json::value& dict,
                   std::string_view key,
                   std::u16string_view value) {
  dict.as_object()[key] = UtfConvert<char>(value);
}

inline void SetKey(boost::json::value& dict,
                   std::string_view key,
                   boost::json::array&& value) {
  dict.as_object()[key] = std::move(value);
}
