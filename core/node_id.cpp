#include "core/node_id.h"

#include <atomic>

#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"

namespace scada {

NodeId::NodeId(String string_id, NamespaceIndex namespace_index)
    : identifier_{std::make_shared<String>(std::move(string_id))},
      namespace_index_{namespace_index} {}

NodeId::NodeId(ByteString opaque_id, NamespaceIndex namespace_index)
    : identifier_{std::make_shared<ByteString>(std::move(opaque_id))},
      namespace_index_{namespace_index} {}

const String& NodeId::string_id() const {
  assert(type() == NodeIdType::String);
  return *std::get<SharedStringId>(identifier_);
}

const ByteString& NodeId::opaque_id() const {
  assert(type() == NodeIdType::Opaque);
  return *std::get<SharedByteString>(identifier_);
}

String NodeId::ToString() const {
  std::string result;

  if (namespace_index_ != 0)
    base::StringAppendF(&result, "ns=%u;",
                        static_cast<unsigned>(namespace_index_));

  switch (type()) {
    case NodeIdType::Numeric:
      base::StringAppendF(&result, "i=%u", static_cast<unsigned>(numeric_id()));
      break;
    case NodeIdType::String:
      base::StringAppendF(&result, "s=%s", string_id().c_str());
      break;
    case NodeIdType::Opaque:
      // TODO:
    default:
      assert(false);
  }

  return result;
}

// static
NodeId NodeId::FromString(const base::StringPiece& string) {
  if (string.empty())
    return {};

  NamespaceIndex namespace_index = 0;

  base::StringPiece str = string;

  if (str.starts_with("ns=")) {
    auto index = str.find(';');
    if (index == base::StringPiece::npos)
      return {};
    unsigned id = 0;
    if (!base::StringToUint(str.substr(3, index - 3), &id))
      return {};
    namespace_index = static_cast<NamespaceIndex>(id);
    str = str.substr(index + 1);
  }

  if (str.starts_with("i=")) {
    unsigned numeric_id = 0;
    if (!base::StringToUint(str.substr(2), &numeric_id))
      return {};
    return {numeric_id, namespace_index};
  }

  if (str.starts_with("s=")) {
    auto string_id = str.substr(2);
    return {string_id.as_string(), namespace_index};
  }

  if (str.starts_with("s=")) {
    auto string_id = str.substr(2);
    return {string_id.as_string(), namespace_index};
  }

  // TODO: g=
  // TODO: b=

  return {str.as_string(), namespace_index};
}

}  // namespace scada
