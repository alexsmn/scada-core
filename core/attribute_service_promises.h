#pragma once

#include "base/promise.h"
#include "core/attribute_service.h"

namespace scada {

inline promise<DataValue> Read(
    AttributeService& attribute_service,
    const std::shared_ptr<const ServiceContext>& context,
    ReadValueId&& input) {
  promise<DataValue> p;
  Read(attribute_service, context, std::move(input),
       [p](DataValue&& status) mutable { p.resolve(std::move(status)); });
  return p;
}

inline promise<Status> Write(
    AttributeService& attribute_service,
    const std::shared_ptr<const ServiceContext>& context,
    WriteValue&& input) {
  promise<Status> p;
  Write(attribute_service, context, std::move(input),
        [p](Status&& status) mutable { p.resolve(std::move(status)); });
  return p;
}

}  // namespace scada