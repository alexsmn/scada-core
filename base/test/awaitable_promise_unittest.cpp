#include "base/awaitable_promise.h"

#include "base/test/asio_test_environment.h"
#include "base/test/test_executor.h"

#include <boost/asio/use_future.hpp>
#include <gtest/gtest.h>
#include <thread>

using namespace std::chrono_literals;

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
    auto result = boost::asio::co_spawn(
        make_adapter(executor),
        AwaitPromise(make_adapter(executor),
                     MakeDeferredValuePromise(executor, i)),
        boost::asio::use_future);

    while (result.wait_for(0ms) != std::future_status::ready) {
      executor->Poll();
    }

    EXPECT_EQ(result.get(), i);
  }
}

template <class AdapterFactory>
void RunCrossThreadDeferredValueAwaitTest(AdapterFactory make_adapter) {
  auto executor = std::make_shared<TestExecutor>();

  for (int i = 0; i < 1000; ++i) {
    promise<int> result;

    auto future = boost::asio::co_spawn(
        make_adapter(executor),
        AwaitPromise(make_adapter(executor), result),
        boost::asio::use_future);

    std::thread resolver{[result, i]() mutable { result.resolve(i); }};

    while (future.wait_for(0ms) != std::future_status::ready) {
      executor->Poll();
    }

    EXPECT_EQ(future.get(), i);
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

template <class T>
T WaitForFuture(AsioTestEnvironment& asio_env, std::future<T>& result) {
  while (result.wait_for(0ms) != std::future_status::ready) {
    asio_env.io_context.run_for(10ms);
    asio_env.io_context.restart();
  }
  return result.get();
}

template <class T>
T WaitForPromise(AsioTestEnvironment& asio_env, promise<T>& result) {
  while (result.wait_for(0ms) == promise_wait_status::timeout) {
    asio_env.io_context.run_for(10ms);
    asio_env.io_context.restart();
  }
  return result.get();
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

}  // namespace

TEST(AwaitPromise, ReturnsResolvedValue) {
  AsioTestEnvironment asio_env;

  auto result = boost::asio::co_spawn(
      asio_env.io_context,
      AwaitPromise(NetExecutorAdapter{asio_env.executor},
                   make_resolved_promise(42)),
      boost::asio::use_future);
  EXPECT_EQ(WaitForFuture(asio_env, result), 42);
}

TEST(AwaitPromise, CompletesDeferredTemporaryValuePromise) {
  AsioTestEnvironment asio_env;

  auto result = boost::asio::co_spawn(
      asio_env.io_context,
      AwaitPromise(NetExecutorAdapter{asio_env.executor},
                   MakeDeferredValuePromise(asio_env.executor, 42)),
      boost::asio::use_future);
  EXPECT_EQ(WaitForFuture(asio_env, result), 42);
}

TEST(ToAwaitable, CompletesDeferredTemporaryVoidPromise) {
  AsioTestEnvironment asio_env;

  auto result = boost::asio::co_spawn(
      asio_env.io_context,
      [](
          const std::shared_ptr<Executor>& executor) -> Awaitable<void> {
        co_await ToAwaitable(MakeDeferredVoidPromise(executor));
      }(asio_env.executor),
      boost::asio::use_future);
  EXPECT_NO_THROW(WaitForFuture(asio_env, result));
}

TEST(ToPromise, CompletesResolvedValueAwaitable) {
  AsioTestEnvironment asio_env;

  auto promise = ToPromise(NetExecutorAdapter{asio_env.executor}, Return42());
  auto result = boost::asio::co_spawn(
      asio_env.io_context,
      AwaitPromise(NetExecutorAdapter{asio_env.executor}, std::move(promise)),
      boost::asio::use_future);

  EXPECT_EQ(WaitForFuture(asio_env, result), 42);
}

TEST(ToPromise, CompletesResolvedVoidAwaitable) {
  AsioTestEnvironment asio_env;

  auto promise =
      ToPromise(NetExecutorAdapter{asio_env.executor}, ReturnVoid());
  auto result = boost::asio::co_spawn(
      asio_env.io_context,
      AwaitPromise(NetExecutorAdapter{asio_env.executor}, std::move(promise)),
      boost::asio::use_future);

  EXPECT_NO_THROW(WaitForFuture(asio_env, result));
}

TEST(ToPromise, CompletesNonDefaultConstructibleAwaitable) {
  AsioTestEnvironment asio_env;

  auto promise = ToPromise(NetExecutorAdapter{asio_env.executor},
                           ReturnNonDefaultConstructible(42));
  EXPECT_EQ(WaitForPromise(asio_env, promise).value, 42);
}

TEST(ToPromise, PropagatesAwaitableExceptions) {
  AsioTestEnvironment asio_env;

  auto promise =
      ToPromise(NetExecutorAdapter{asio_env.executor}, ThrowRuntimeError());
  auto result = boost::asio::co_spawn(
      asio_env.io_context,
      AwaitPromise(NetExecutorAdapter{asio_env.executor}, std::move(promise)),
      boost::asio::use_future);

  EXPECT_THROW(WaitForFuture(asio_env, result), std::runtime_error);
}

TEST(StartAwaitable, CompletesResolvedValueAwaitableIntoProvidedPromise) {
  AsioTestEnvironment asio_env;

  promise<int> promise;
  StartAwaitable(NetExecutorAdapter{asio_env.executor}, promise, Return42());

  EXPECT_EQ(WaitForPromise(asio_env, promise), 42);
}

TEST(StartAwaitable, PropagatesAwaitableExceptionsIntoProvidedPromise) {
  AsioTestEnvironment asio_env;

  promise<int> promise;
  StartAwaitable(NetExecutorAdapter{asio_env.executor}, promise,
                 ThrowRuntimeError());

  EXPECT_THROW(WaitForPromise(asio_env, promise), std::runtime_error);
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

TEST(AwaitPromise, ExecutorAdapterReusesCanonicalStatePerExecutor) {
  RunCanonicalAdapterStateTest(
      [](const std::shared_ptr<Executor>& executor) {
        return ExecutorAdapter{executor};
      });
}

TEST(AwaitPromise, RepeatedDeferredTemporaryValuePromiseWithAsioExecutor) {
  boost::asio::io_context io_context;

  for (int i = 0; i < 1000; ++i) {
    promise<int> result;
    boost::asio::post(
        io_context,
        [result, i]() mutable { result.resolve(i); });

    auto future = boost::asio::co_spawn(
        io_context, AwaitPromise(io_context.get_executor(), std::move(result)),
        boost::asio::use_future);

    while (future.wait_for(0ms) != std::future_status::ready) {
      io_context.run_for(10ms);
      io_context.restart();
    }

    EXPECT_EQ(future.get(), i);
  }
}
