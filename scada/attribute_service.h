#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/executor_conversions.h"
#include "scada/callback_awaitable.h"
#include "scada/data_value.h"
#include "scada/node_class.h"
#include "scada/read_value_id.h"
#include "scada/service_context.h"
#include "scada/status_callback.h"
#include "scada/status_exception.h"
#include "scada/status_or.h"
#include "scada/write_flags.h"

#include <cassert>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace scada {

class ServiceContext;

using ReadCallback =
    std::function<void(Status, std::vector<DataValue> results)>;

using WriteCallback = MultiStatusCallback;

struct WriteValue {
  NodeId node_id;
  AttributeId attribute_id = scada::AttributeId::Value;
  Variant value;
  WriteFlags flags;

  bool operator==(const WriteValue&) const = default;
};

class AttributeService {
 public:
  virtual ~AttributeService() = default;

  virtual void Read(
      const ServiceContext& context,
      const std::shared_ptr<const std::vector<ReadValueId>>& inputs,
      const ReadCallback& callback) = 0;

  virtual void Write(
      const ServiceContext& context,
      const std::shared_ptr<const std::vector<WriteValue>>& inputs,
      const WriteCallback& callback) = 0;
};

class CoroutineAttributeService {
 public:
  virtual ~CoroutineAttributeService() = default;

  virtual Awaitable<StatusOr<std::vector<DataValue>>> Read(
      ServiceContext context,
      std::shared_ptr<const std::vector<ReadValueId>> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>> Write(
      ServiceContext context,
      std::shared_ptr<const std::vector<WriteValue>> inputs) = 0;
};

class CallbackToCoroutineAttributeServiceAdapter final
    : public CoroutineAttributeService {
 public:
  CallbackToCoroutineAttributeServiceAdapter(AnyExecutor executor,
                                             AttributeService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CallbackToCoroutineAttributeServiceAdapter(std::shared_ptr<Executor> executor,
                                             AttributeService& service)
      : CallbackToCoroutineAttributeServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  Awaitable<StatusOr<std::vector<DataValue>>> Read(
      ServiceContext context,
      std::shared_ptr<const std::vector<ReadValueId>> inputs) override {
    co_return co_await AwaitStatusOrCallback<std::vector<DataValue>>(
        executor_,
        [this, context = std::move(context), inputs = std::move(inputs)](
            auto callback) mutable {
          service_.Read(context, std::move(inputs), std::move(callback));
        });
  }

  Awaitable<StatusOr<std::vector<StatusCode>>> Write(
      ServiceContext context,
      std::shared_ptr<const std::vector<WriteValue>> inputs) override {
    co_return co_await AwaitStatusOrCallback<std::vector<StatusCode>>(
        executor_,
        [this, context = std::move(context), inputs = std::move(inputs)](
            auto callback) mutable {
          service_.Write(context, std::move(inputs), std::move(callback));
        });
  }

 private:
  const AnyExecutor executor_;
  AttributeService& service_;
};

class CoroutineToCallbackAttributeServiceAdapter final
    : public AttributeService {
 public:
  CoroutineToCallbackAttributeServiceAdapter(AnyExecutor executor,
                                             CoroutineAttributeService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CoroutineToCallbackAttributeServiceAdapter(std::shared_ptr<Executor> executor,
                                             CoroutineAttributeService& service)
      : CoroutineToCallbackAttributeServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  void Read(const ServiceContext& context,
            const std::shared_ptr<const std::vector<ReadValueId>>& inputs,
            const ReadCallback& callback) override {
    CoSpawn(executor_,
            [this, context, inputs, callback]() mutable -> Awaitable<void> {
              try {
                CompleteStatusOrCallback(
                    callback,
                    co_await service_.Read(context, std::move(inputs)));
              } catch (...) {
                callback(GetExceptionStatus(std::current_exception()), {});
              }
            });
  }

  void Write(const ServiceContext& context,
             const std::shared_ptr<const std::vector<WriteValue>>& inputs,
             const WriteCallback& callback) override {
    CoSpawn(executor_,
            [this, context, inputs, callback]() mutable -> Awaitable<void> {
              try {
                CompleteStatusOrCallback(
                    callback,
                    co_await service_.Write(context, std::move(inputs)));
              } catch (...) {
                auto status = GetExceptionStatus(std::current_exception());
                callback(std::move(status), {});
              }
            });
  }

 private:
  const AnyExecutor executor_;
  CoroutineAttributeService& service_;
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

// callback: void(DataValue&&)
template <class Callback>
inline void Read(AttributeService& attribute_service,
                 const scada::ServiceContext& context,
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
                  const scada::ServiceContext& context,
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

inline std::ostream& operator<<(std::ostream& stream,
                                const WriteValue& value_id) {
  return stream << "{"
                << "node_id: " << value_id.node_id
                << ", attribute_id: " << value_id.attribute_id
                << ", value: " << value_id.value << "}";
}

}  // namespace scada
