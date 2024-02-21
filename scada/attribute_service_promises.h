#pragma once

#include "scada/attribute_service.h"
#include "scada/status_promise.h"

namespace scada {

// Prefer `scada::client{...}.node(input.node_id).read(input.attribute_id)`.
inline status_promise<DataValue> Read(
    AttributeService& attribute_service,
    const std::shared_ptr<const ServiceContext>& context,
    ReadValueId&& input) {
  status_promise<DataValue> p;
  Read(attribute_service, context, std::move(input),
       MakeStatusCodePromiseCallback(p));
  return p;
}

inline status_promise<std::vector<StatusCode>> Write(
    AttributeService& attribute_service,
    const std::shared_ptr<const ServiceContext>& context,
    std::vector<WriteValue> inputs) {
  status_promise<std::vector<StatusCode>> p;
  attribute_service.Write(
      context, std::make_shared<std::vector<WriteValue>>(std::move(inputs)),
      [p](Status status, std::vector<StatusCode> results) mutable {
        if (status) {
          p.resolve(std::move(results));
        } else {
          RejectStatusPromise(p, std::move(status));
        }
      });
  return p;
}

inline status_promise<void> Write(
    AttributeService& attribute_service,
    const std::shared_ptr<const ServiceContext>& context,
    WriteValue&& input) {
  status_promise<void> p;
  Write(attribute_service, context, std::move(input),
        MakeStatusPromiseCallback(p));
  return p;
}

}  // namespace scada