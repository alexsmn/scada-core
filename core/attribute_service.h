#pragma once

#include "core/attribute_ids.h"
#include "core/data_value.h"
#include "core/node_class.h"
#include "core/node_id.h"
#include "core/write_flags.h"

#include <cassert>
#include <functional>

namespace scada {

class Status;

using StatusCallback = std::function<void(Status&&)>;
using MultiStatusCallback =
    std::function<void(Status&&, std::vector<StatusCode>&&)>;

struct ReadValueId {
  NodeId node_id;
  AttributeId attribute_id;
};

using ReadCallback =
    std::function<void(Status&&, std::vector<DataValue>&& results)>;
using WriteCallback = MultiStatusCallback;

struct WriteValueId {
  NodeId node_id;
  AttributeId attribute_id;
  Variant value;
  WriteFlags flags;
};

class AttributeService {
 public:
  virtual ~AttributeService() {}

  virtual void Read(const std::vector<ReadValueId>& value_ids,
                    const ReadCallback& callback) = 0;

  virtual void Write(const std::vector<WriteValueId>& value_ids,
                     const NodeId& user_id,
                     const WriteCallback& callback) = 0;
};

template <class T>
inline DataValue MakeReadResult(T&& value) {
  const auto timestamp = base::Time::Now();
  return DataValue{std::forward<T>(value), {}, timestamp, timestamp};
}

inline DataValue MakeReadResult(NodeClass node_class) {
  return MakeReadResult(static_cast<int>(node_class));
}

inline DataValue MakeReadError(StatusCode status_code) {
  assert(IsBad(status_code));
  const auto timestamp = base::Time::Now();
  return DataValue{status_code, timestamp};
}

inline bool operator==(const ReadValueId& a, const ReadValueId& b) {
  return std::tie(a.node_id, a.attribute_id) ==
         std::tie(b.node_id, b.attribute_id);
}

inline bool operator==(const WriteValueId& a, const WriteValueId& b) {
  return std::tie(a.node_id, a.attribute_id, a.value, a.flags) ==
         std::tie(b.node_id, b.attribute_id, b.value, b.flags);
}

inline std::ostream& operator<<(std::ostream& stream, const ReadValueId& v) {
  return stream << "{" << v.node_id << ", " << v.attribute_id << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const WriteValueId& value_id) {
  return stream << "{"
                << "node_id: " << value_id.node_id
                << ", attribute_id: " << value_id.attribute_id
                << ", value: " << value_id.value << "}";
}

}  // namespace scada
