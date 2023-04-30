#pragma once

#include "base/values.h"

#include <boost/json.hpp>

inline base::Value ToValue(boost::json::value value) {
  if (value.is_null()) {
    return base::Value{};
  } else if (value.is_bool()) {
    return base::Value{value.as_bool()};
  } else if (value.is_int64()) {
    return base::Value{static_cast<int>(value.as_int64())};
  } else if (value.is_uint64()) {
    return base::Value{static_cast<int>(value.as_uint64())};
  } else if (value.is_double()) {
    return base::Value{value.as_double()};
  } else if (value.is_string()) {
    return base::Value{std::string{value.as_string()}};
  } else if (value.is_array()) {
    base::Value::ListStorage list;
    for (const auto& item : value.as_array()) {
      list.emplace_back(ToValue(item));
    }
    return base::Value{std::move(list)};
  } else if (value.is_object()) {
    base::Value::DictStorage dict;
    for (const auto& [key, item] : value.as_object()) {
      dict.try_emplace(key, base::Value::ToUniquePtrValue(ToValue(item)));
    }
    return base::Value{std::move(dict)};
  } else {
    NOTREACHED();
    return base::Value{};
  }
}
