#include "base/value_util.h"

#include "base/utf_convert.h"

std::u16string GetString16(const base::Value& value,
                           std::string_view key,
                           std::u16string_view default_value) {
  if (!value.is_dict())
    return std::u16string{default_value};
  if (auto* k = value.FindKeyOfType(key, base::Value::Type::STRING)) {
    return UtfConvert<char16_t>(k->GetString());
  } else {
    return std::u16string{default_value};
  }
}

const base::Value& GetDict(const base::Value& value, std::string_view key) {
  const static base::Value kEmptyDict{base::Value::Type::DICTIONARY};
  const auto* dict = FindDict(value, key);
  return dict ? *dict : kEmptyDict;
}

const base::Value& GetKey(const base::Value& dict, std::string_view key) {
  const static base::Value kNullValue;
  auto* k = dict.FindKey(key);
  return k ? *k : kNullValue;
}
