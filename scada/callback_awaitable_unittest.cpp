#include "scada/callback_awaitable.h"

#include "base/awaitable_promise.h"
#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/status_callback.h"

#include <gtest/gtest.h>

using namespace std::chrono_literals;

namespace scada {
namespace {

TEST(ScadaCallbackAwaitable, AwaitsDeferredStatusCallback) {
  auto executor = std::make_shared<TestExecutor>();

  StatusCallback callback;
  auto status_promise = ToPromise(
      MakeTestAnyExecutor(executor),
      AwaitStatusCallback(
          MakeTestAnyExecutor(executor),
          [&](StatusCallback done) { callback = std::move(done); }));

  Drain(executor);
  EXPECT_EQ(status_promise.wait_for(0ms), promise_wait_status::timeout);
  ASSERT_TRUE(callback);

  callback(StatusCode::Bad_Disconnected);

  EXPECT_EQ(WaitPromise(executor, std::move(status_promise)).code(),
            StatusCode::Bad_Disconnected);
}

TEST(ScadaCallbackAwaitable, AwaitsStatusCodesCallback) {
  auto executor = std::make_shared<TestExecutor>();

  auto result_promise = ToPromise(
      MakeTestAnyExecutor(executor),
      AwaitStatusCodesCallback(MakeTestAnyExecutor(executor),
                               [](auto done) mutable {
                                 done(Status{StatusCode::Good},
                                      std::vector<StatusCode>{
                                          StatusCode::Good,
                                          StatusCode::Bad_WrongAttributeId});
                               }));

  auto result = WaitPromise(executor, std::move(result_promise));
  ASSERT_TRUE(result.ok());
  EXPECT_EQ(*result,
            (std::vector<StatusCode>{StatusCode::Good,
                                     StatusCode::Bad_WrongAttributeId}));
}

TEST(ScadaCallbackAwaitable, AwaitsSingleCallbackValue) {
  auto executor = std::make_shared<TestExecutor>();

  auto value_promise =
      ToPromise(MakeTestAnyExecutor(executor),
                AwaitCallbackValue<int>(MakeTestAnyExecutor(executor),
                                        [](auto done) mutable { done(42); }));

  EXPECT_EQ(WaitPromise(executor, std::move(value_promise)), 42);
}

}  // namespace
}  // namespace scada
