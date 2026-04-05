#include "base/promise_executor.h"

#include "base/test/test_executor.h"

#include <gmock/gmock.h>

using namespace testing;

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

TEST(BindPromiseCancelation, RejectsCanceledPromiseWithValueHandler) {
  promise<int> canceled_promise = make_resolved_promise().then(
      BindPromiseCancelation(std::weak_ptr<int>{}, [] { return 123; }));

  EXPECT_THROW(canceled_promise.get(), std::exception);
}

TEST(BindPromiseCancelation, RejectsCanceledPromiseWithPromiseHandler) {
  promise<int> canceled_promise =
      make_resolved_promise().then(BindPromiseCancelation(
          std::weak_ptr<int>{}, [] { return make_resolved_promise(123); }));

  EXPECT_THROW(canceled_promise.get(), std::exception);
}

TEST(BindPromiseExecutor, FuncReturnsVoid) {
  auto executor = std::make_shared<TestExecutor>();

  MockFunction<void()> func;

  std::function<promise<void>()> binding =
      BindPromiseExecutor(executor, func.AsStdFunction());

  EXPECT_CALL(func, Call()).WillOnce(Invoke([&] {
    EXPECT_TRUE(executor->is_current_executor());
  }));

  binding().get();
}

TEST(BindPromiseExecutor, FuncReturnsValue) {
  auto executor = std::make_shared<TestExecutor>();

  MockFunction<int()> func;

  std::function<promise<int>()> binding =
      BindPromiseExecutor(executor, func.AsStdFunction());

  EXPECT_CALL(func, Call()).WillOnce(Invoke([&] {
    EXPECT_TRUE(executor->is_current_executor());
    return 42;
  }));

  EXPECT_EQ(binding().get(), 42);
}

TEST(BindPromiseExecutor, FuncReturnsVoidPromise) {
  auto executor = std::make_shared<TestExecutor>();

  MockFunction<promise<void>()> func;

  std::function<promise<void>()> binding =
      BindPromiseExecutor(executor, func.AsStdFunction());

  EXPECT_CALL(func, Call()).WillOnce(Invoke([&] {
    EXPECT_TRUE(executor->is_current_executor());
    return make_resolved_promise();
  }));

  binding().get();
}

TEST(BindPromiseExecutor, FuncReturnsValuePromise) {
  auto executor = std::make_shared<TestExecutor>();

  MockFunction<promise<int>()> func;

  std::function<promise<int>()> binding =
      BindPromiseExecutor(executor, func.AsStdFunction());

  EXPECT_CALL(func, Call()).WillOnce(Invoke([&] {
    EXPECT_TRUE(executor->is_current_executor());
    return make_resolved_promise(42);
  }));

  EXPECT_EQ(binding().get(), 42);
}

TEST(BindPromiseExecutor, WithCancelation_FuncReturnsVoid) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  MockFunction<void()> func;

  std::function<promise<void>()> binding = BindPromiseExecutor(
      executor, std::weak_ptr{cancelation}, func.AsStdFunction());

  EXPECT_CALL(func, Call()).WillOnce(Invoke([&] {
    EXPECT_TRUE(executor->is_current_executor());
    return make_resolved_promise(42);
  }));

  binding();
}

TEST(BindPromiseExecutor, WithCancelation_FuncReturnsVoid_Canceled) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  MockFunction<void()> func;

  std::function<promise<void>()> binding = BindPromiseExecutor(
      executor, std::weak_ptr{cancelation}, func.AsStdFunction());

  EXPECT_CALL(func, Call()).Times(0);

  cancelation.reset();

  EXPECT_THROW(binding().get(), std::exception);
}

TEST(BindPromiseExecutor, WithCancelation_FuncReturnsValue_Canceled) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  MockFunction<int()> func;

  std::function<promise<int>()> binding = BindPromiseExecutor(
      executor, std::weak_ptr{cancelation}, func.AsStdFunction());

  EXPECT_CALL(func, Call()).Times(0);

  cancelation.reset();

  EXPECT_THROW(binding().get(), std::exception);
}

TEST(BindPromiseExecutor, WithCancelation_FuncReturnsVoidPromise) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  MockFunction<promise<void>()> func;

  std::function<promise<void>()> binding = BindPromiseExecutor(
      executor, std::weak_ptr{cancelation}, func.AsStdFunction());

  EXPECT_CALL(func, Call()).WillOnce(Invoke([&] {
    EXPECT_TRUE(executor->is_current_executor());
    return make_resolved_promise();
  }));

  binding().get();
}

TEST(BindPromiseExecutor, WithCancelation_FuncReturnsValuePromise) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  MockFunction<promise<int>()> func;

  std::function<promise<int>()> binding = BindPromiseExecutor(
      executor, std::weak_ptr{cancelation}, func.AsStdFunction());

  EXPECT_CALL(func, Call()).WillOnce(Invoke([&] {
    EXPECT_TRUE(executor->is_current_executor());
    return make_resolved_promise(42);
  }));

  binding().get();
}

TEST(BindPromiseExecutor, WithCancelation_FuncReturnsValuePromise_Canceled) {
  auto executor = std::make_shared<TestExecutor>();
  auto cancelation = std::make_shared<int>();

  std::function<promise<int>()> binding =
      BindPromiseExecutor(executor, std::weak_ptr{cancelation}, [&] {
        EXPECT_TRUE(executor->is_current_executor());
        return make_resolved_promise(42);
      });

  cancelation.reset();

  EXPECT_THROW(binding().get(), std::exception);
}