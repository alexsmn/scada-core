#include "base/promise_executor.h"

#include "base/test/test_executor.h"

#include <gmock/gmock.h>

TEST(DispatchPromise, Test) {
  auto executor = std::make_shared<TestExecutor>();

  auto dispatched_promise = DispatchPromise(*executor, [&] {
    EXPECT_TRUE(executor->is_current_executor());
    return make_resolved_promise(42);
  });

  EXPECT_EQ(dispatched_promise.get(), 42);
}

TEST(BindPromiseExecutor, ResolverReturnsConstant) {
  auto executor = std::make_shared<TestExecutor>();
  std::function<promise<int>()> binding =
      BindPromiseExecutor(executor, [] { return 42; });

  EXPECT_EQ(binding().get(), 42);
}

/*TEST(BindPromiseExecutorWithResult, ResolverReturnsConstant) {
  auto executor = std::make_shared<TestExecutor>();
  std::function<promise<void>()> binding =
      BindPromiseExecutorWithResult(executor, [] { return 42; });

  binding().get();
}*/

TEST(BindPromiseExecutorWithResult, ResolverReturnsVoidPromise) {
  auto executor = std::make_shared<TestExecutor>();

  std::function<promise<void>()> binding =
      BindPromiseExecutorWithResult(executor, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return make_resolved_promise();
      });

  binding().get();
}

TEST(BindPromiseExecutorWithResult, ResolverReturnsValuePromise) {
  auto executor = std::make_shared<TestExecutor>();

  std::function<promise<int>()> binding =
      BindPromiseExecutorWithResult(executor, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return make_resolved_promise(42);
      });

  EXPECT_EQ(binding().get(), 42);
}

TEST(BindPromiseExecutorWithResult, ResolverReturnsVoidPromiseWithCancelation) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  std::function<promise<void>()> binding =
      BindPromiseExecutorWithResult(executor, std::weak_ptr{cancelation}, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return make_resolved_promise();
      });

  binding().get();
}

TEST(BindPromiseExecutorWithResult,
     ResolverReturnsValuePromiseWithCancelation) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  std::function<promise<int>()> binding =
      BindPromiseExecutorWithResult(executor, std::weak_ptr{cancelation}, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return make_resolved_promise(42);
      });

  EXPECT_EQ(binding().get(), 42);
}