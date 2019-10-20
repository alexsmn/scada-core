#pragma once

#include "core/attribute_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockAttributeService : public AttributeService {
 public:
  MOCK_METHOD3(
      Read,
      void(const std::shared_ptr<const scada::ServiceContext>& context,
           const std::shared_ptr<const std::vector<scada::ReadValueId>>& inputs,
           const scada::ReadCallback& callback));

  MOCK_METHOD3(
      Write,
      void(
          const std::shared_ptr<const scada::ServiceContext>& context,
          const std::shared_ptr<const std::vector<scada::WriteValueId>>& inputs,
          const scada::WriteCallback& callback));
};

class SimpleMockAttributeService : public AttributeService {
 public:
  MOCK_METHOD1(Read, DataValue(const ReadValueId& value_id));

  MOCK_METHOD2(Write, Status(const WriteValue& value, const NodeId& user_id));

  virtual void Read(
      const std::shared_ptr<const scada::ServiceContext>& context,
      const std::shared_ptr<const std::vector<scada::ReadValueId>>& inputs,
      const scada::ReadCallback& callback) override {
    std::vector<DataValue> results(inputs->size());
    for (size_t i = 0; i < inputs->size(); ++i)
      results[i] = Read((*inputs)[i]);
    callback(StatusCode::Good, std::move(results));
  }

  virtual void Write(
      const std::shared_ptr<const scada::ServiceContext>& context,
      const std::shared_ptr<const std::vector<scada::WriteValueId>>& inputs,
      const scada::WriteCallback& callback) override {
    std::vector<Status> results(inputs->size());
    for (size_t i = 0; i < inputs->size(); ++i)
      results[i] = Write((*inputs)[i], context->user_id);
    callback(StatusCode::Good, std::move(results));
  }
};

}  // namespace scada
