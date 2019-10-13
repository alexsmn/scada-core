#pragma once

#include "base/containers/span.h"
#include "core/attribute_ids.h"
#include "core/configuration_types.h"
#include "core/data_value.h"
#include "core/node_class.h"
#include "core/write_flags.h"

#include <cassert>
#include <functional>

namespace scada {

class NodeId;
class Status;

using StatusCallback = std::function<void(Status&&)>;
using MultiStatusCallback =
    std::function<void(Status&&, std::vector<Status>&&)>;
using ReadCallback =
    std::function<void(Status&&, std::vector<DataValue>&& values)>;

struct WriteValue {
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

  virtual void Write(base::span<const WriteValue> values,
                     const NodeId& user_id,
                     const MultiStatusCallback& callback) = 0;
};

template <class T>
inline DataValue MakeReadResult(T&& value) {
  const auto timestamp = base::Time::Now();
  return DataValue{std::forward<T>(value), {}, timestamp, timestamp};
}

inline DataValue MakeReadResult(scada::NodeClass node_class) {
  return MakeReadResult(static_cast<int>(node_class));
}

inline DataValue MakeReadError(scada::StatusCode status_code) {
  assert(scada::IsBad(status_code));
  const auto timestamp = base::Time::Now();
  return DataValue{status_code, timestamp};
}

}  // namespace scada

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::WriteValue& write_value) {
  return stream << "{"
                << "node_id: " << write_value.node_id
                << ", attribute_id: " << write_value.attribute_id
                << ", value: " << write_value.value << "}";
}