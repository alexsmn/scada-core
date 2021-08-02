#pragma once

#include "core/standard_node_ids.h"
#include "core/variant.h"

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

template <class T>
inline scada::NodeId ToBuiltInDataType();

template <>
inline scada::NodeId ToBuiltInDataType<scada::Int8>() {
  return scada::id::Int8;
}

template <>
inline scada::NodeId ToBuiltInDataType<scada::UInt8>() {
  return scada::id::UInt8;
}

template <>
inline scada::NodeId ToBuiltInDataType<scada::Int16>() {
  return scada::id::Int16;
}

template <>
inline scada::NodeId ToBuiltInDataType<scada::UInt16>() {
  return scada::id::UInt16;
}

template <>
inline scada::NodeId ToBuiltInDataType<scada::Int32>() {
  return scada::id::Int32;
}

template <>
inline scada::NodeId ToBuiltInDataType<scada::UInt32>() {
  return scada::id::UInt32;
}

template <>
inline scada::NodeId ToBuiltInDataType<scada::Int64>() {
  return scada::id::Int64;
}

template <>
inline scada::NodeId ToBuiltInDataType<scada::UInt64>() {
  return scada::id::UInt64;
}

template <>
inline scada::NodeId ToBuiltInDataType<scada::Double>() {
  return scada::id::Double;
}

}  // namespace scada
