#include "scada/node_id.h"

#include <boost/algorithm/string/predicate.hpp>

#include <format>
#include <limits>

#include <atomic>
#include <boost/container_hash/hash.hpp>
#include <optional>

#if defined(SCADA_USE_BASE_MODULE)
// Modules-pilot consumer (SCADA_CXX_MODULES=ON): base names come from the
// scada.base facade. The import sits after the textual includes because the
// reverse order trips an AppleClang 21 declaration-merging bug in libc++.
import scada.base;
#else
#include "base/base64.h"
#include "base/check.h"
#include "base/format.h"
#endif

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
    case scada::NodeIdType::Guid: {
      const scada::Guid& guid = node_id.guid_id();
      boost::hash_combine(seed, guid.data1);
      boost::hash_combine(seed, guid.data2);
      boost::hash_combine(seed, guid.data3);
      boost::hash_combine(seed, guid.data4);
      break;
    }
    default:
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
  base::Check(source.is_null());
}

NodeId& NodeId::operator=(NodeId&& source) noexcept {
  if (this != &source) {
    namespace_index_ = source.namespace_index_;
    identifier_ = std::move(source.identifier_);
    source.identifier_ = {};
    source.namespace_index_ = 0;
    base::Check(source.is_null());
  }
  return *this;
}

const String& NodeId::string_id() const {
  base::Check(type() == NodeIdType::String);
  return std::get<SharedValue<String>>(identifier_).get();
}

const ByteString& NodeId::opaque_id() const {
  base::Check(type() == NodeIdType::Opaque);
  return std::get<SharedValue<ByteString>>(identifier_).get();
}

String NodeId::ToString() const {
  std::string result;

  if (namespace_index_ != 0) {
    result += std::format("ns={};", static_cast<unsigned>(namespace_index_));
  }

  switch (type()) {
    case NodeIdType::Numeric:
      result += std::format("i={}", static_cast<unsigned>(numeric_id()));
      break;
    case NodeIdType::String:
      result += std::format("s={}", string_id());
      break;
    case NodeIdType::Opaque: {
      std::string encoded;
      base::Base64Encode(
          std::string_view{opaque_id().data(), opaque_id().size()}, &encoded);
      result += std::format("b={}", encoded);
      break;
    }
    case NodeIdType::Guid:
      result += std::format("g={}", guid_id().ToString());
      break;
    default:
      result += "<invalid>";
  }

  return result;
}

// static
NodeId NodeId::FromString(std::string_view string) {
  if (string.empty())
    return {};

  NamespaceIndex namespace_index = 0;

  std::string_view str = string;

  if (boost::istarts_with(str, "ns=")) {
    auto index = str.find(';');
    if (index == std::string_view::npos)
      return {};
    unsigned id = 0;
    if (!Parse(str.substr(3, index - 3), id))
      return {};
    // NamespaceIndex is 16 bits (scada/basic_types.h). Narrowing a larger
    // value would silently name a different namespace — "ns=70000" became
    // ns=4464 — so it is rejected like any other malformed id.
    if (id > std::numeric_limits<NamespaceIndex>::max())
      return {};
    namespace_index = static_cast<NamespaceIndex>(id);
    str = str.substr(index + 1);
  }

  if (boost::istarts_with(str, "i=")) {
    unsigned numeric_id = 0;
    if (!Parse(str.substr(2), numeric_id)) {
      return {};
    }
    return {numeric_id, namespace_index};
  }

  if (boost::istarts_with(str, "s=")) {
    auto string_id = str.substr(2);
    return {std::string{string_id}, namespace_index};
  }

  // OPC UA Part 6 §5.3.1.10 NodeId, the g= and b= forms,
  // https://reference.opcfoundation.org/Core/Part6/v105/docs/5.3.1.10
  if (boost::istarts_with(str, "g=")) {
    const std::optional<Guid> guid = Guid::FromString(str.substr(2));
    if (!guid.has_value())
      return {};
    return {*guid, namespace_index};
  }

  if (boost::istarts_with(str, "b=")) {
    std::string decoded;
    if (!base::Base64Decode(str.substr(2), &decoded))
      return {};
    return {ByteString{decoded.begin(), decoded.end()}, namespace_index};
  }

  return {std::string{str}, namespace_index};
}

}  // namespace scada
