#include "base/io_thread.h"

#include "base/any_executor.h"
#include "base/common_types.h"

#include <boost/asio/post.hpp>
#include <boost/asio/steady_timer.hpp>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

using namespace std::chrono_literals;

namespace scada::base {
namespace {

// Waits for `predicate` rather than sleeping a fixed amount, so the tests stay
// off the clock on a loaded machine.
template <class Predicate>
bool WaitFor(Predicate predicate, std::chrono::milliseconds timeout = 5s) {
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  while (!predicate()) {
    if (std::chrono::steady_clock::now() >= deadline)
      return false;
    std::this_thread::sleep_for(1ms);
  }
  return true;
}

TEST(IoThreadTest, RunsPostedWorkOnItsOwnThread) {
  IoThread io_thread{"scada-io-test"};
  std::atomic<bool> ran = false;
  std::thread::id ran_on;

  boost::asio::post(io_thread.context(), [&] {
    ran_on = std::this_thread::get_id();
    ran = true;
  });

  ASSERT_TRUE(WaitFor([&] { return ran.load(); }));
  EXPECT_NE(ran_on, std::this_thread::get_id());
}

// The work guard is what keeps the thread alive across idle gaps. Without it
// `run()` returns as soon as the first burst drains and every later operation
// silently never completes.
TEST(IoThreadTest, SurvivesAnIdlePeriod) {
  IoThread io_thread{"scada-io-test"};
  std::atomic<int> ran = 0;

  boost::asio::post(io_thread.context(), [&] { ++ran; });
  ASSERT_TRUE(WaitFor([&] { return ran.load() == 1; }));

  std::this_thread::sleep_for(50ms);

  boost::asio::post(io_thread.context(), [&] { ++ran; });
  EXPECT_TRUE(WaitFor([&] { return ran.load() == 2; }));
}

TEST(IoThreadTest, ServicesTimers) {
  IoThread io_thread{"scada-io-test"};
  boost::asio::steady_timer timer{io_thread.context()};
  timer.expires_after(20ms);

  std::atomic<bool> fired = false;
  timer.async_wait([&](boost::system::error_code) { fired = true; });

  EXPECT_TRUE(WaitFor([&] { return fired.load(); }));
}

// Destruction must not wait for outstanding work: an armed timer that will not
// expire for an hour, or a socket read that never completes, would otherwise
// hang shutdown.
TEST(IoThreadTest, DestructorDoesNotWaitForOutstandingWork) {
  const auto started = std::chrono::steady_clock::now();
  {
    IoThread io_thread{"scada-io-test"};
    auto timer = std::make_shared<boost::asio::steady_timer>(
        io_thread.context(), std::chrono::hours{1});
    timer->async_wait([timer](boost::system::error_code) {});
  }
  EXPECT_LT(std::chrono::steady_clock::now() - started, 2s);
}

TEST(IoThreadTest, SetCurrentThreadNameToleratesAnOverlongName) {
  // Linux rejects anything past 16 bytes; the helper must truncate rather than
  // silently drop the name, and must not throw on any platform.
  EXPECT_NO_THROW(
      SetCurrentThreadName("a-name-far-longer-than-any-platform-allows"));
  EXPECT_NO_THROW(SetCurrentThreadName(""));
}

// A queue-draining stand-in for MessageLoopQt: enough of an executor for
// MakeAnyExecutor, with no Qt dependency.
class QueueExecutor {
 public:
  void PostDelayedTask(Clock::duration delay,
                       std::function<void()> task,
                       const std::source_location& = {}) {
    std::lock_guard lock{mutex_};
    queue_.emplace(std::move(task));
  }

  bool RunOne() {
    std::function<void()> task;
    {
      std::lock_guard lock{mutex_};
      if (queue_.empty())
        return false;
      task = std::move(queue_.front());
      queue_.pop();
    }
    task();
    return true;
  }

 private:
  std::mutex mutex_;
  std::queue<std::function<void()>> queue_;
};

// The contract that made this class necessary: an executor built from a plain
// task queue must expose a real io_context that something is already running.
// Backed by a bare execution_context, asio instead spawns an unnamed thread of
// its own on first use, and there is nothing to post to here at all.
//
// Note the test never drains `queue`, so the owned I/O thread is the only thing
// that can run the handler.
TEST(IoThreadTest, ExecutorsContextIsRunByAnOwnedThread) {
  auto queue = std::make_shared<QueueExecutor>();
  AnyExecutor executor = MakeAnyExecutor(queue);

  auto& io_context = static_cast<boost::asio::io_context&>(
      boost::asio::query(executor, boost::asio::execution::context));

  std::atomic<bool> ran = false;
  std::thread::id ran_on;
  boost::asio::post(io_context, [&] {
    ran_on = std::this_thread::get_id();
    ran = true;
  });

  ASSERT_TRUE(WaitFor([&] { return ran.load(); }));
  EXPECT_NE(ran_on, std::this_thread::get_id());
}

// ...and asio objects built on that executor still complete, with their
// handlers marshalled back onto the task queue rather than run on the I/O
// thread.
TEST(IoThreadTest, HandlersFromTheExecutorRunOnTheQueue) {
  auto queue = std::make_shared<QueueExecutor>();
  AnyExecutor executor = MakeAnyExecutor(queue);

  boost::asio::steady_timer timer{executor};
  timer.expires_after(10ms);

  std::atomic<bool> queued = false;
  std::thread::id ran_on;
  timer.async_wait([&](boost::system::error_code) {
    ran_on = std::this_thread::get_id();
    queued = true;
  });

  // The handler only runs when the queue is drained, on the draining thread.
  ASSERT_TRUE(WaitFor([&] { return queue->RunOne(); }));
  EXPECT_TRUE(queued);
  EXPECT_EQ(ran_on, std::this_thread::get_id());
}

}  // namespace
}  // namespace scada::base
