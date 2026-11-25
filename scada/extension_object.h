#pragma once

#include "base/lifetime.h"
#include "scada/basic_types.h"
#include "scada/expanded_node_id.h"

#include <any>
#include <optional>
#include <ostream>

namespace scada {

class ExtensionObject;

// Encodes an ExtensionObject's payload in a canonical binary form, or returns
// nullopt when it cannot. Core encodes no payload type itself; the OPC UA
// bridge installs one built on the generated codec
// (opcua_bridge::InstallExtensionObjectEncoder).
using ExtensionObjectEncoder =
    std::optional<ByteString> (*)(const ExtensionObject& object);

// Installs the encoder ExtensionObject::operator== compares payloads with. Set
// once at process start-up; pass nullptr to remove it.
void SetExtensionObjectEncoder(ExtensionObjectEncoder encoder);

class ExtensionObject {
 public:
  ExtensionObject() {}

  ExtensionObject(ExpandedNodeId data_type_id, std::any value)
      : data_type_id_{std::move(data_type_id)}, value_{std::move(value)} {}

  const ExpandedNodeId& data_type_id() const SCADA_LIFETIME_BOUND {
    return data_type_id_;
  }

  ExtensionObject(const ExtensionObject&) = default;
  ExtensionObject& operator=(const ExtensionObject&) = default;

  ExtensionObject(ExtensionObject&& source) noexcept
      : data_type_id_{std::move(source.data_type_id_)},
        value_{std::move(source.value_)} {}

  ExtensionObject& operator=(ExtensionObject&& source) noexcept {
    if (&value_ != &source.value_) {
      data_type_id_ = std::move(source.data_type_id_);
      value_ = std::move(source.value_);
    }
    return *this;
  }

  // Equal when the data type ids match and either neither holds a payload or
  // both payloads encode to the same bytes with the installed
  // ExtensionObjectEncoder. A payload the encoder cannot encode, or any payload
  // when no encoder is installed, compares unequal — the only answer a
  // std::any allows. This used to be unequal always, even to itself, so a
  // Variant holding RolePermissions never compared equal and every re-read
  // reported a change.
  bool operator==(const ExtensionObject& other) const;

  /*template<class T>
  static ExtensionObject Encode(ExpandedNodeId data_type_id, T&& object) {
    return ExtensionObject{std::any{std::move(object)}};
  }*/

  std::any& value() SCADA_LIFETIME_BOUND { return value_; }
  const std::any& value() const SCADA_LIFETIME_BOUND { return value_; }

 private:
  ExpandedNodeId data_type_id_;
  std::any value_;
};

inline std::ostream& operator<<(std::ostream& stream,
                                const ExtensionObject& extension_object) {
  return stream << "{data_type_id: " << extension_object.data_type_id() << "}";
}

}  // namespace scada
