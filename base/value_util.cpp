#include "base/value_util.h"

#include <boost/locale/encoding_utf.hpp>

std::u16string GetString16(const base::Value& value,
                           std::string_view key,
                           std::u16string_view default_value) {
  if (!value.is_dict())
    return std::u16string{default_value};
  if (auto* k =
          value.FindKeyOfType(AsStringPiece(key), base::Value::Type::STRING)) {
    return boost::locale::conv::utf_to_utf<char16_t>(k->GetString());
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
  auto* k = dict.FindKey(AsStringPiece(key));
  return k ? *k : kNullValue;
}
