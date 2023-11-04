#include "base/promise_executor.h"

#include "base/test/test_executor.h"

#include <gmock/gmock.h>

TEST(BindPromiseExecutorWithResult, VoidReturnType) {
  auto executor = std::make_shared<TestExecutor>();
  std::function<promise<void>()> binding = BindPromiseExecutorWithResult(
      executor, [] { return make_resolved_promise(); });

  binding().get();
}

TEST(BindPromiseExecutorWithResult, IntReturnType) {
  auto executor = std::make_shared<TestExecutor>();
  std::function<promise<int>()> binding = BindPromiseExecutorWithResult(
      executor, [] { return make_resolved_promise(42); });

  EXPECT_EQ(binding().get(), 42);
}

TEST(BindPromiseExecutorWithResult, VoidReturnTypeWithCancelation) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();
  std::function<promise<void>()> binding =
      BindPromiseExecutorWithResult(executor, std::weak_ptr{cancelation},
                                    [] { return make_resolved_promise(); });

  binding().get();
}

TEST(BindPromiseExecutorWithResult, IntReturnTypeWithCancelation) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();
  std::function<promise<int>()> binding =
      BindPromiseExecutorWithResult(executor, std::weak_ptr{cancelation},
                                    [] { return make_resolved_promise(42); });

  EXPECT_EQ(binding().get(), 42);
}