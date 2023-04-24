#pragma once

#include "core/attribute_ids.h"
#include "core/data_value.h"
#include "core/node_class.h"
#include "core/node_id.h"
#include "core/service.h"
#include "core/status_callback.h"
#include "core/write_flags.h"

#include <cassert>
#include <functional>

namespace scada {

struct ReadValueId {
  NodeId node_id;
  AttributeId attribute_id = scada::AttributeId::Value;
};

using ReadCallback =
    std::function<void(Status&&, std::vector<DataValue>&& results)>;
using WriteCallback = MultiStatusCallback;

struct WriteValue {
  NodeId node_id;
  AttributeId attribute_id = scada::AttributeId::Value;
  Variant value;
  WriteFlags flags;
};

class AttributeService {
 public:
  virtual ~AttributeService() = default;

  virtual void Read(
      const std::shared_ptr<const ServiceContext>& context,
      const std::shared_ptr<const std::vector<ReadValueId>>& inputs,
      const ReadCallback& callback) = 0;

  virtual void Write(
      const std::shared_ptr<const ServiceContext>& context,
      const std::shared_ptr<const std::vector<WriteValue>>& inputs,
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

template <class Callback>
inline void Read(AttributeService& attribute_service,
                 const std::shared_ptr<const scada::ServiceContext>& context,
                 ReadValueId&& input,
                 Callback&& callback) {
  auto inputs = std::make_shared<std::vector<ReadValueId>>(1, std::move(input));
  attribute_service.Read(
      context, inputs,
      [callback = std::forward<Callback>(callback)](
          Status&& status, std::vector<DataValue>&& results) mutable {
        assert(!status || results.size() == 1);
        callback(status ? std::move(results[0]) : MakeReadError(status.code()));
      });
}

template <class Callback>
inline void Write(AttributeService& attribute_service,
                  const std::shared_ptr<const scada::ServiceContext>& context,
                  WriteValue&& input,
                  Callback&& callback) {
  auto inputs = std::make_shared<std::vector<WriteValue>>(1, std::move(input));
  attribute_service.Write(
      context, inputs,
      [callback = std::forward<Callback>(callback)](
          Status&& status, std::vector<StatusCode>&& results) mutable {
        assert(!status || results.size() == 1);
        callback(status ? Status{results[0]} : std::move(status));
      });
}

inline bool operator==(const ReadValueId& a, const ReadValueId& b) {
  return std::tie(a.node_id, a.attribute_id) ==
         std::tie(b.node_id, b.attribute_id);
}

inline bool operator==(const WriteValue& a, const WriteValue& b) {
  return std::tie(a.node_id, a.attribute_id, a.value, a.flags) ==
         std::tie(b.node_id, b.attribute_id, b.value, b.flags);
}

inline std::ostream& operator<<(std::ostream& stream, const ReadValueId& v) {
  return stream << "{" << v.node_id << ", " << v.attribute_id << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const WriteValue& value_id) {
  return stream << "{"
                << "node_id: " << value_id.node_id
                << ", attribute_id: " << value_id.attribute_id
                << ", value: " << value_id.value << "}";
}

}  // namespace scada
