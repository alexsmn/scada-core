#pragma once

#include "core/attribute_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockAttributeService : public AttributeService {
 public:
  MOCK_METHOD(void,
              Read,
              (const std::vector<ReadValueId>& value_ids,
               const ReadCallback& callback));

  MOCK_METHOD(void,
              Write,
              (const std::vector<WriteValueId>& value_ids,
               const NodeId& user_id,
               const WriteCallback& callback));
};

class SimpleMockAttributeService final : public AttributeService {
 public:
  MOCK_METHOD(DataValue, Read, (const ReadValueId& value_id));

  MOCK_METHOD(StatusCode,
              Write,
              (const WriteValueId& value_id, const NodeId& user_id));

  virtual void Read(const std::vector<ReadValueId>& value_ids,
                    const ReadCallback& callback) override {
    std::vector<DataValue> results(value_ids.size());
    for (size_t i = 0; i < value_ids.size(); ++i)
      results[i] = Read(value_ids[i]);
    callback(StatusCode::Good, std::move(results));
  }

  virtual void Write(const std::vector<WriteValueId>& value_ids,
                     const NodeId& user_id,
                     const WriteCallback& callback) override {
    std::vector<StatusCode> results(value_ids.size());
    for (size_t i = 0; i < value_ids.size(); ++i)
      results[i] = Write(value_ids[i], user_id);
    callback(StatusCode::Good, std::move(results));
  }
};

}  // namespace scada
