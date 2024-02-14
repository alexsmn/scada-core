#include "base/promise_executor.h"

#include "base/test/test_executor.h"

#include <gmock/gmock.h>

TEST(DispatchAsPromise, ClosureReturnsVoid) {
  auto executor = std::make_shared<TestExecutor>();

  auto dispatched_promise = DispatchAsPromise(
      *executor, [&] { EXPECT_TRUE(executor->is_current_executor()); });

  dispatched_promise.get();
}

TEST(DispatchAsPromise, ClosureReturnsValue) {
  auto executor = std::make_shared<TestExecutor>();

  auto dispatched_promise = DispatchAsPromise(*executor, [&] {
    EXPECT_TRUE(executor->is_current_executor());
    return 42;
  });

  EXPECT_EQ(dispatched_promise.get(), 42);
}

TEST(DispatchAsPromise, ClosureReturnsPromise) {
  auto executor = std::make_shared<TestExecutor>();

  auto dispatched_promise = DispatchAsPromise(*executor, [&] {
    EXPECT_TRUE(executor->is_current_executor());
    return make_resolved_promise(42);
  });

  EXPECT_EQ(dispatched_promise.get(), 42);
}

TEST(BindPromiseExecutor, FuncReturnsVoid) {
  auto executor = std::make_shared<TestExecutor>();

  std::function<promise<void>()> binding = BindPromiseExecutor(
      executor, [&] { EXPECT_TRUE(executor->is_current_executor()); });

  binding().get();
}

TEST(BindPromiseExecutor, FuncReturnsValue) {
  auto executor = std::make_shared<TestExecutor>();

  std::function<promise<int>()> binding = BindPromiseExecutor(executor, [&] {
    EXPECT_TRUE(executor->is_current_executor());
    return 42;
  });

  EXPECT_EQ(binding().get(), 42);
}

#if 0
TEST(BindPromiseExecutorWithResult, FuncReturnsValue) {
  auto executor = std::make_shared<TestExecutor>();

  std::function<promise<void>()> binding =
      BindPromiseExecutorWithResult(executor, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return 42;
      });

  binding().get();
}
#endif

TEST(BindPromiseExecutor, WithCancelation_FuncReturnsVoid) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  std::function<promise<void>()> binding = BindPromiseExecutor(
      executor, std::weak_ptr{cancelation},
      [&] { EXPECT_TRUE(executor->is_current_executor()); });

  binding().get();
}

TEST(BindPromiseExecutorWithResult, FuncReturnsVoidPromise) {
  auto executor = std::make_shared<TestExecutor>();

  std::function<promise<void>()> binding =
      BindPromiseExecutorWithResult(executor, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return make_resolved_promise();
      });

  binding().get();
}

TEST(BindPromiseExecutorWithResult, FuncReturnsValuePromise) {
  auto executor = std::make_shared<TestExecutor>();

  std::function<promise<int>()> binding =
      BindPromiseExecutorWithResult(executor, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return make_resolved_promise(42);
      });

  EXPECT_EQ(binding().get(), 42);
}

TEST(BindPromiseExecutorWithResult, WithCancelation_FuncReturnsVoidPromise) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  std::function<promise<void>()> binding =
      BindPromiseExecutorWithResult(executor, std::weak_ptr{cancelation}, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return make_resolved_promise();
      });

  binding().get();
}

TEST(BindPromiseExecutorWithResult, WithCancelation_FuncReturnsValuePromise) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  std::function<promise<int>()> binding =
      BindPromiseExecutorWithResult(executor, std::weak_ptr{cancelation}, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return make_resolved_promise(42);
      });

  EXPECT_EQ(binding().get(), 42);
}