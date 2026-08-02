#include "scada/callback_awaitable.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/status_callback.h"

#include <gtest/gtest.h>

using namespace std::chrono_literals;

namespace scada {
namespace {

TEST(ScadaCallbackAwaitable, AwaitsDeferredStatusCallback) {
  TestExecutor executor;

  StatusCallback callback;
  auto status_result = StartAwaitable(
      executor,
      AwaitStatusCallback(
          executor,
          [&](StatusCallback done) { callback = std::move(done); }));

  Drain(executor);
  EXPECT_FALSE(status_result->done);
  ASSERT_TRUE(callback);

  callback(StatusCode::Bad_Disconnected);

  EXPECT_EQ(WaitResult(executor, status_result).code(),
            StatusCode::Bad_Disconnected);
}

TEST(ScadaCallbackAwaitable, AwaitsStatusCodesCallback) {
  TestExecutor executor;

  auto result =
      WaitAwaitable(executor,
      AwaitStatusCodesCallback(executor,
                               [](auto done) mutable {
                                 done(Status{StatusCode::Good},
                                      std::vector<StatusCode>{
                                          StatusCode::Good,
                                          StatusCode::Bad_WrongAttributeId});
                               }));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(*result,
            (std::vector<StatusCode>{StatusCode::Good,
                                     StatusCode::Bad_WrongAttributeId}));
}

TEST(ScadaCallbackAwaitable, AwaitsSingleCallbackValue) {
  TestExecutor executor;

  EXPECT_EQ(WaitAwaitable(executor,
                          AwaitCallbackValue<int>(
                              executor,
                              [](auto done) mutable { done(42); })),
            42);
}

}  // namespace
}  // namespace scada
