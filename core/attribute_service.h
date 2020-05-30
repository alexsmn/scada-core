#pragma once

#include "core/attribute_ids.h"
#include "core/configuration_types.h"
#include "core/data_value.h"
#include "core/node_id.h"
#include "core/write_flags.h"

#include <cassert>
#include <functional>

namespace scada {

class Status;

struct ReadValueId {
  NodeId node_id;
  AttributeId attribute_id;
};

struct WriteValue {
  NodeId node_id;
  AttributeId attribute_id;
  Variant value;
  WriteFlags flags;
};

using WriteValueId = WriteValue;

using StatusCallback = std::function<void(Status&&)>;
using ReadCallback =
    std::function<void(Status&&, std::vector<DataValue>&& values)>;

using WriteCallback =
    std::function<void(Status&& status,
                       std::vector<StatusCode>&& status_codes)>;

class AttributeService {
 public:
  virtual ~AttributeService() {}

  virtual void Read(const std::vector<ReadValueId>& value_ids,
                    const ReadCallback& callback) = 0;

  virtual void Write(const std::vector<WriteValue>& value_ids,
                     const NodeId& user_id,
                     const WriteCallback& callback) = 0;
};

template <class T>
inline DataValue MakeReadResult(T&& value) {
  const auto timestamp = base::Time::Now();
  return DataValue{std::forward<T>(value), {}, timestamp, timestamp};
}

inline DataValue MakeReadError(scada::StatusCode status_code) {
  assert(scada::IsBad(status_code));
  const auto timestamp = base::Time::Now();
  return DataValue{status_code, timestamp};
}

}  // namespace scada

inline bool operator==(const scada::ReadValueId& a,
                       const scada::ReadValueId& b) {
  return std::tie(a.node_id, a.attribute_id) ==
         std::tie(b.node_id, b.attribute_id);
}
