#include "base/awaitable_promise.h"
#include "base/callback_awaitable.h"

#include "base/test/asio_test_environment.h"
#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <thread>

namespace {

promise<int> MakeDeferredValuePromise(const std::shared_ptr<Executor>& executor,
                                      int value) {
  promise<void> trigger;
  promise<int> result;

  Dispatch(*executor, [trigger]() mutable { trigger.resolve(); });
  trigger.then([result, value]() mutable { result.resolve(value); });

  return result;
}

promise<void> MakeDeferredVoidPromise(const std::shared_ptr<Executor>& executor) {
  promise<void> trigger;
  promise<void> result;

  Dispatch(*executor, [trigger]() mutable { trigger.resolve(); });
  trigger.then([result]() mutable { result.resolve(); });

  return result;
}

template <class AdapterFactory>
void RunRepeatedDeferredValueAwaitTest(AdapterFactory make_adapter) {
  auto executor = std::make_shared<TestExecutor>();

  for (int i = 0; i < 1000; ++i) {
    auto result = ToPromise(
        make_adapter(executor),
        AwaitPromise(make_adapter(executor),
                     MakeDeferredValuePromise(executor, i)));

    EXPECT_EQ(WaitPromise(executor, std::move(result)), i);
  }
}

template <class AdapterFactory>
void RunCrossThreadDeferredValueAwaitTest(AdapterFactory make_adapter) {
  auto executor = std::make_shared<TestExecutor>();

  for (int i = 0; i < 1000; ++i) {
    promise<int> source;

    auto waiter = ToPromise(
        make_adapter(executor),
        AwaitPromise(make_adapter(executor), source));

    std::thread resolver{[source, i]() mutable { source.resolve(i); }};

    EXPECT_EQ(WaitPromise(executor, std::move(waiter)), i);
    resolver.join();
  }
}

template <class AdapterFactory>
void RunCanonicalAdapterStateTest(AdapterFactory make_adapter) {
  auto executor = std::make_shared<TestExecutor>();

  auto first = make_adapter(executor);
  auto second = make_adapter(executor);

  EXPECT_EQ(first, second);
  EXPECT_EQ(
      &first.query(boost::asio::execution::context),
      &second.query(boost::asio::execution::context));
}

struct NonDefaultConstructible {
  explicit NonDefaultConstructible(int value) : value{value} {}

  int value;
};

Awaitable<int> Return42() {
  co_return 42;
}

Awaitable<void> ReturnVoid() {
  co_return;
}

Awaitable<NonDefaultConstructible> ReturnNonDefaultConstructible(int value) {
  co_return NonDefaultConstructible{value};
}

Awaitable<int> ThrowRuntimeError() {
  throw std::runtime_error{"boom"};
  co_return 0;
}

promise<int> MakeTestExecutorDeferredValuePromise(
    const std::shared_ptr<TestExecutor>& executor,
    int value) {
  promise<int> result;
  Dispatch(*executor, [result, value]() mutable { result.resolve(value); });
  return result;
}

promise<void> MakeTestExecutorDeferredVoidPromise(
    const std::shared_ptr<TestExecutor>& executor) {
  promise<void> result;
  Dispatch(*executor, [result]() mutable { result.resolve(); });
  return result;
}

Awaitable<int> ReturnDeferredValue(const std::shared_ptr<TestExecutor>& executor,
                                   int value) {
  co_await CallbackToAwaitable(
      MakeTestAnyExecutor(executor),
      [executor](auto callback) mutable {
        Dispatch(*executor, [callback = std::move(callback)]() mutable {
          callback();
        });
      });
  co_return value;
}

}  // namespace

TEST(AwaitPromise, ReturnsResolvedValue) {
  AsioTestEnvironment asio_env;

  EXPECT_EQ(asio_env.Wait(AwaitPromise(NetExecutorAdapter{asio_env.executor},
                                       make_resolved_promise(42))),
            42);
}

TEST(AwaitPromise, CompletesDeferredTemporaryValuePromise) {
  AsioTestEnvironment asio_env;

  EXPECT_EQ(asio_env.Wait(AwaitPromise(
                NetExecutorAdapter{asio_env.executor},
                MakeDeferredValuePromise(asio_env.executor, 42))),
            42);
}

TEST(ToAwaitable, CompletesDeferredTemporaryVoidPromise) {
  AsioTestEnvironment asio_env;

  EXPECT_NO_THROW(asio_env.Wait(
      [](const std::shared_ptr<Executor>& executor) -> Awaitable<void> {
        co_await ToAwaitable(MakeDeferredVoidPromise(executor));
      }(asio_env.executor)));
}

TEST(ToPromise, CompletesResolvedValueAwaitable) {
  AsioTestEnvironment asio_env;

  auto promise = ToPromise(NetExecutorAdapter{asio_env.executor}, Return42());

  EXPECT_EQ(asio_env.Wait(AwaitPromise(NetExecutorAdapter{asio_env.executor},
                                       std::move(promise))),
            42);
}

TEST(ToPromise, CompletesResolvedVoidAwaitable) {
  AsioTestEnvironment asio_env;

  auto promise =
      ToPromise(NetExecutorAdapter{asio_env.executor}, ReturnVoid());

  EXPECT_NO_THROW(asio_env.Wait(
      AwaitPromise(NetExecutorAdapter{asio_env.executor}, std::move(promise))));
}

TEST(ToPromise, CompletesNonDefaultConstructibleAwaitable) {
  AsioTestEnvironment asio_env;

  auto promise = ToPromise(NetExecutorAdapter{asio_env.executor},
                           ReturnNonDefaultConstructible(42));
  EXPECT_EQ(asio_env.Wait(promise).value, 42);
}

TEST(ToPromise, PropagatesAwaitableExceptions) {
  AsioTestEnvironment asio_env;

  auto promise =
      ToPromise(NetExecutorAdapter{asio_env.executor}, ThrowRuntimeError());

  EXPECT_THROW(asio_env.Wait(AwaitPromise(NetExecutorAdapter{asio_env.executor},
                                          std::move(promise))),
               std::runtime_error);
}

TEST(WaitPromiseHelper, CompletesDeferredValuePromiseOnTestExecutor) {
  auto executor = std::make_shared<TestExecutor>();

  EXPECT_EQ(WaitPromise(executor, MakeTestExecutorDeferredValuePromise(executor,
                                                                       42)),
            42);
}

TEST(WaitPromiseHelper, CompletesDeferredVoidPromiseOnTestExecutor) {
  auto executor = std::make_shared<TestExecutor>();

  EXPECT_NO_THROW(
      WaitPromise(executor, MakeTestExecutorDeferredVoidPromise(executor)));
}

TEST(WaitPromiseHelper, CompletesValuePromiseWithCustomDrain) {
  promise<int> resolver;
  promise<int> waiter = resolver;
  int drain_count = 0;

  EXPECT_EQ(WaitPromise(std::move(waiter),
                        [&] {
                          ++drain_count;
                          resolver.resolve(42);
                        }),
            42);
  EXPECT_EQ(drain_count, 1);
}

TEST(WaitPromiseHelper, CompletesVoidPromiseWithCustomDrain) {
  promise<void> resolver;
  promise<void> waiter = resolver;
  int drain_count = 0;

  EXPECT_NO_THROW(WaitPromise(std::move(waiter), [&] {
    ++drain_count;
    resolver.resolve();
  }));
  EXPECT_EQ(drain_count, 1);
}

TEST(WaitAwaitableHelper, CompletesDeferredAwaitableOnTestExecutor) {
  auto executor = std::make_shared<TestExecutor>();

  EXPECT_EQ(WaitAwaitable(executor, ReturnDeferredValue(executor, 42)), 42);
}

TEST(WaitAwaitableHelper, PropagatesAwaitableExceptionsOnTestExecutor) {
  auto executor = std::make_shared<TestExecutor>();

  EXPECT_THROW(WaitAwaitable(executor, ThrowRuntimeError()),
               std::runtime_error);
}

TEST(StartAwaitable, CompletesResolvedValueAwaitableIntoProvidedPromise) {
  AsioTestEnvironment asio_env;

  promise<int> promise;
  StartAwaitable(NetExecutorAdapter{asio_env.executor}, promise, Return42());

  EXPECT_EQ(asio_env.Wait(promise), 42);
}

TEST(StartAwaitable, PropagatesAwaitableExceptionsIntoProvidedPromise) {
  AsioTestEnvironment asio_env;

  promise<int> promise;
  StartAwaitable(NetExecutorAdapter{asio_env.executor}, promise,
                 ThrowRuntimeError());

  EXPECT_THROW(asio_env.Wait(promise), std::runtime_error);
}

TEST(AwaitPromise, RepeatedDeferredTemporaryValuePromiseWithNetExecutorAdapter) {
  RunRepeatedDeferredValueAwaitTest(
      [](const std::shared_ptr<Executor>& executor) {
        return NetExecutorAdapter{executor};
      });
}

TEST(AwaitPromise, RepeatedDeferredTemporaryValuePromiseWithExecutorAdapter) {
  RunRepeatedDeferredValueAwaitTest(
      [](const std::shared_ptr<Executor>& executor) {
        return ExecutorAdapter{executor};
      });
}

TEST(AwaitPromise, CrossThreadDeferredTemporaryValuePromiseWithNetExecutorAdapter) {
  RunCrossThreadDeferredValueAwaitTest(
      [](const std::shared_ptr<Executor>& executor) {
        return NetExecutorAdapter{executor};
      });
}

TEST(AwaitPromise, CrossThreadDeferredTemporaryValuePromiseWithExecutorAdapter) {
  RunCrossThreadDeferredValueAwaitTest(
      [](const std::shared_ptr<Executor>& executor) {
        return ExecutorAdapter{executor};
      });
}

TEST(AwaitPromise, NetExecutorAdapterReusesCanonicalStatePerExecutor) {
  RunCanonicalAdapterStateTest(
      [](const std::shared_ptr<Executor>& executor) {
        return NetExecutorAdapter{executor};
      });
}

TEST(AwaitPromise, NetExecutorAdapterDoesNotReuseStateForDifferentOwners) {
  auto* raw_executor = new TestExecutor;
  auto noop_delete = [](Executor*) {};

  {
    auto first_owner = std::shared_ptr<Executor>{raw_executor, noop_delete};
    auto first = NetExecutorAdapter{first_owner};
    first_owner.reset();

    auto second_owner = std::shared_ptr<Executor>{raw_executor, noop_delete};
    auto second = NetExecutorAdapter{second_owner};

    EXPECT_NE(first, second);
    EXPECT_NE(&first.query(boost::asio::execution::context),
              &second.query(boost::asio::execution::context));
  }

  delete raw_executor;
}

TEST(AwaitPromise, ExecutorAdapterReusesCanonicalStatePerExecutor) {
  RunCanonicalAdapterStateTest(
      [](const std::shared_ptr<Executor>& executor) {
        return ExecutorAdapter{executor};
      });
}

TEST(AwaitPromise, ExecutorAdapterDoesNotReuseStateForDifferentOwners) {
  auto* raw_executor = new TestExecutor;
  auto noop_delete = [](Executor*) {};

  {
    auto first_owner = std::shared_ptr<Executor>{raw_executor, noop_delete};
    auto first = ExecutorAdapter{first_owner};
    first_owner.reset();

    auto second_owner = std::shared_ptr<Executor>{raw_executor, noop_delete};
    auto second = ExecutorAdapter{second_owner};

    EXPECT_NE(first, second);
    EXPECT_NE(&first.query(boost::asio::execution::context),
              &second.query(boost::asio::execution::context));
  }

  delete raw_executor;
}

TEST(AwaitPromise, RepeatedDeferredTemporaryValuePromiseWithAsioExecutor) {
  boost::asio::io_context io_context;

  for (int i = 0; i < 1000; ++i) {
    promise<int> result;
    boost::asio::post(
        io_context,
        [result, i]() mutable { result.resolve(i); });

    EXPECT_EQ(
        RunAwaitable(io_context,
                     [executor = io_context.get_executor(),
                      result = std::move(result)]() mutable -> Awaitable<int> {
                       co_return co_await AwaitPromise(executor,
                                                       std::move(result));
                     }),
        i);
  }
}
