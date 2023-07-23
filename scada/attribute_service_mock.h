#pragma once

#include "scada/attribute_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockAttributeService : public AttributeService {
 public:
  MOCK_METHOD(void,
              Read,
              (const std::shared_ptr<const ServiceContext>& context,
               const std::shared_ptr<const std::vector<ReadValueId>>& inputs,
               const ReadCallback& callback),
              (override));

  MOCK_METHOD(void,
              Write,
              (const std::shared_ptr<const ServiceContext>& context,
               const std::shared_ptr<const std::vector<WriteValue>>& inputs,
               const WriteCallback& callback),
              (override));
};

class SimpleMockAttributeService : public AttributeService {
 public:
  SimpleMockAttributeService() {
    using namespace testing;

    ON_CALL(*this, Read(_))
        .WillByDefault(Return(MakeReadError(StatusCode::Bad)));

    ON_CALL(*this, Write(_, _)).WillByDefault(Return(StatusCode::Good));
  }

  MOCK_METHOD(DataValue, Read, (const ReadValueId& value_id));

  MOCK_METHOD(StatusCode,
              Write,
              (const std::shared_ptr<const ServiceContext>& context,
               const WriteValue& value));

  virtual void Read(
      const std::shared_ptr<const ServiceContext>& context,
      const std::shared_ptr<const std::vector<ReadValueId>>& inputs,
      const ReadCallback& callback) override {
    std::vector<DataValue> results(inputs->size());
    for (size_t i = 0; i < inputs->size(); ++i)
      results[i] = Read((*inputs)[i]);
    callback(StatusCode::Good, std::move(results));
  }

  virtual void Write(
      const std::shared_ptr<const ServiceContext>& context,
      const std::shared_ptr<const std::vector<WriteValue>>& inputs,
      const WriteCallback& callback) override {
    std::vector<StatusCode> results(inputs->size());
    for (size_t i = 0; i < inputs->size(); ++i)
      results[i] = Write(context, (*inputs)[i]);
    callback(StatusCode::Good, std::move(results));
  }
};

}  // namespace scada
