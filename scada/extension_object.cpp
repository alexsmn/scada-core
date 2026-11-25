#include "scada/extension_object.h"

namespace scada {

namespace {

// Set once at start-up and read thereafter; a function-local static keeps it
// out of static-global-init ordering.
ExtensionObjectEncoder& GetExtensionObjectEncoder() {
  static ExtensionObjectEncoder encoder = nullptr;
  return encoder;
}

}  // namespace

void SetExtensionObjectEncoder(ExtensionObjectEncoder encoder) {
  GetExtensionObjectEncoder() = encoder;
}

bool ExtensionObject::operator==(const ExtensionObject& other) const {
  if (data_type_id_ != other.data_type_id_)
    return false;
  if (!value_.has_value() || !other.value_.has_value())
    return !value_.has_value() && !other.value_.has_value();

  const ExtensionObjectEncoder encoder = GetExtensionObjectEncoder();
  if (!encoder)
    return false;
  const std::optional<ByteString> body = encoder(*this);
  const std::optional<ByteString> other_body = encoder(other);
  return body.has_value() && other_body.has_value() && *body == *other_body;
}

}  // namespace scada
