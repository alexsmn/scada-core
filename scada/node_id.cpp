#include "scada/node_id.h"

#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"

#include <atomic>
#include <boost/container_hash/hash.hpp>

namespace std {

std::size_t hash<scada::NodeId>::operator()(
    const scada::NodeId& node_id) const noexcept {
  std::size_t seed = 0;
  boost::hash_combine(seed, node_id.namespace_index());
  boost::hash_combine(seed, node_id.type());
  switch (node_id.type()) {
    case scada::NodeIdType::Numeric:
      boost::hash_combine(seed, node_id.numeric_id());
      break;
    case scada::NodeIdType::String:
      boost::hash_combine(seed, node_id.string_id());
      break;
    case scada::NodeIdType::Opaque:
      boost::hash_combine(seed, node_id.opaque_id());
      break;
    default:
      assert(false);
      break;
  }
  return seed;
}

}  // namespace std

namespace scada {

NodeId::NodeId(String string_id, NamespaceIndex namespace_index)
    : identifier_{SharedValue<String>{std::move(string_id)}},
      namespace_index_{namespace_index} {}

NodeId::NodeId(ByteString opaque_id, NamespaceIndex namespace_index)
    : identifier_{SharedValue<ByteString>(std::move(opaque_id))},
      namespace_index_{namespace_index} {}

NodeId::NodeId(NodeId&& source) noexcept
    : identifier_{std::move(source.identifier_)},
      namespace_index_{source.namespace_index_} {
  source.identifier_ = static_cast<NumericId>(0);
  source.namespace_index_ = 0;
  assert(source.is_null());
}

NodeId& NodeId::operator=(NodeId&& source) noexcept {
  if (this != &source) {
    namespace_index_ = source.namespace_index_;
    identifier_ = std::move(source.identifier_);
    source.identifier_ = {};
    source.namespace_index_ = 0;
    assert(source.is_null());
  }
  return *this;
}

const String& NodeId::string_id() const {
  assert(type() == NodeIdType::String);
  return std::get<SharedValue<String>>(identifier_).get();
}

const ByteString& NodeId::opaque_id() const {
  assert(type() == NodeIdType::Opaque);
  return std::get<SharedValue<ByteString>>(identifier_).get();
}

String NodeId::ToString() const {
  std::string result;

  if (namespace_index_ != 0) {
    base::StringAppendF(&result, "ns=%u;",
                        static_cast<unsigned>(namespace_index_));
  }

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
NodeId NodeId::FromString(std::string_view string) {
  if (string.empty())
    return {};

  NamespaceIndex namespace_index = 0;

  std::string_view str = string;

  if (base::StartsWith(str, "ns=", base::CompareCase::INSENSITIVE_ASCII)) {
    auto index = str.find(';');
    if (index == std::string_view::npos)
      return {};
    unsigned id = 0;
    if (!base::StringToUint(str.substr(3, index - 3), &id))
      return {};
    namespace_index = static_cast<NamespaceIndex>(id);
    str = str.substr(index + 1);
  }

  if (base::StartsWith(str, "i=", base::CompareCase::INSENSITIVE_ASCII)) {
    unsigned numeric_id = 0;
    if (!base::StringToUint(str.substr(2), &numeric_id)) {
      return {};
    }
    return {numeric_id, namespace_index};
  }

  if (base::StartsWith(str, "s=", base::CompareCase::INSENSITIVE_ASCII)) {
    auto string_id = str.substr(2);
    return {std::string{string_id}, namespace_index};
  }

  if (base::StartsWith(str, "s=", base::CompareCase::INSENSITIVE_ASCII)) {
    auto string_id = str.substr(2);
    return {std::string{string_id}, namespace_index};
  }

  // TODO: g=
  // TODO: b=

  return {std::string{str}, namespace_index};
}

}  // namespace scada
