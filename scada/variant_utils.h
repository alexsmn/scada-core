#pragma once

#include "scada/variant.h"

namespace scada {

template <class ValueType>
inline bool ConvertVariant(const Variant& source, ValueType& target) {
  return source.get(target);
}

template <>
inline bool ConvertVariant(const Variant& source, Variant& target) {
  target = source;
  return true;
}

template <>
bool ConvertVariant(const Variant& source, std::wstring& target);

template <class T>
inline scada::Variant::Type ToBuiltInDataType();

template <>
inline scada::Variant::Type ToBuiltInDataType<scada::Int8>() {
  return scada::Variant::Type::INT8;
}

template <>
inline scada::Variant::Type ToBuiltInDataType<scada::UInt8>() {
  return scada::Variant::Type::UINT8;
}

template <>
inline scada::Variant::Type ToBuiltInDataType<scada::Int16>() {
  return scada::Variant::Type::INT16;
}

template <>
inline scada::Variant::Type ToBuiltInDataType<scada::UInt16>() {
  return scada::Variant::Type::UINT16;
}

template <>
inline scada::Variant::Type ToBuiltInDataType<scada::Int32>() {
  return scada::Variant::Type::INT32;
}

template <>
inline scada::Variant::Type ToBuiltInDataType<scada::UInt32>() {
  return scada::Variant::Type::UINT32;
}

template <>
inline scada::Variant::Type ToBuiltInDataType<scada::Int64>() {
  return scada::Variant::Type::INT64;
}

template <>
inline scada::Variant::Type ToBuiltInDataType<scada::UInt64>() {
  return scada::Variant::Type::UINT64;
}

template <>
inline scada::Variant::Type ToBuiltInDataType<scada::Double>() {
  return scada::Variant::Type::DOUBLE;
}

}  // namespace scada
