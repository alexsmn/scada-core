// Pins the gmock behaviour that makes a capturing lambda coroutine unsafe as
// a `WillRepeatedly` action, and safe under `WillOnce` and `WillByDefault`.
// Read in GoogleTest 1.17.0 (the version vcpkg resolves here, verified
// 2026-09-27):
// https://raw.githubusercontent.com/google/googletest/v1.17.0/googlemock/include/gmock/gmock-spec-builders.h
// See CLAUDE.md, "Unit Test Guidance", and
// `tools/build/check_gmock_coroutine_actions.py`, whose scope follows from
// exactly these cases: if a gmock upgrade changes any of them, this test goes
// red and the check's scope has to be revisited.

#include "base/awaitable.h"
#include "base/test/awaitable_test.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <set>

namespace scada {
namespace {

using testing::Invoke;

// Records which `CaptureTracker` objects are alive, by address only.
class LiveTrackers {
 public:
  void Add(const void* tracker) { live_.insert(tracker); }
  void Remove(const void* tracker) { live_.erase(tracker); }
  bool IsLive(const void* tracker) const { return live_.contains(tracker); }

 private:
  std::set<const void*> live_;
};

// A lambda capture that registers every copy of itself while it is alive, so
// a coroutine body can ask whether the closure it runs against still exists
// without reading any of that closure's memory: it passes only the address of
// its own capture, which is computed from the frame's closure pointer.
class CaptureTracker {
 public:
  explicit CaptureTracker(LiveTrackers& trackers) : trackers_{&trackers} {
    trackers_->Add(this);
  }
  CaptureTracker(const CaptureTracker& other) : trackers_{other.trackers_} {
    trackers_->Add(this);
  }
  CaptureTracker& operator=(const CaptureTracker&) = delete;
  ~CaptureTracker() { trackers_->Remove(this); }

 private:
  LiveTrackers* trackers_;
};

// A mock with one lazy-awaitable method, which is the shape of every service
// mock in the tree. The registry arrives as an argument because a by-value
// coroutine parameter is copied into the frame; reaching it through a
// by-reference capture would itself read the closure under test.
class MockProbe {
 public:
  MOCK_METHOD(Awaitable<bool>, Run, (const LiveTrackers* trackers));
};

// The action is written the way backlog 742 found it written: a capturing
// lambda coroutine. It must not touch the capture's memory, so it answers
// whether the capture's own address is still registered as alive.
auto MakeLifetimeReportingAction(LiveTrackers& trackers) {
  CaptureTracker tracker{trackers};
  return [tracker](const LiveTrackers* live) -> Awaitable<bool> {
    co_return live->IsLive(&tracker);
  };
}

// `WillOnce` smuggles its callable into the expectation behind a
// `shared_ptr<OnceAction>`, so gmock's copy of the action copies that pointer
// and the closure lives as long as the expectation does. Safe, provided the
// awaitable is awaited before the mock is destroyed.
TEST(GmockAwaitableActionLifetime,
     ExpectCallOnceActionRunsAgainstALiveClosure) {
  TestExecutor executor;
  LiveTrackers trackers;
  MockProbe probe;
  EXPECT_CALL(probe, Run(testing::_))
      .WillOnce(Invoke(MakeLifetimeReportingAction(trackers)));

  Awaitable<bool> pending = probe.Run(&trackers);
  EXPECT_TRUE(WaitAwaitable(executor, std::move(pending)));
}

// `WillRepeatedly` stores the callable by value in a `std::function`, and
// gmock's `PerformAction` copies an expectation's action before performing it
// ("in case the action deletes the mock object"). The copy dies when the call
// returns; the awaitable is lazy, so its body runs after that, against a
// closure that no longer exists. This is the shape the check flags.
TEST(GmockAwaitableActionLifetime,
     ExpectCallRepeatedActionRunsAgainstADeadClosure) {
  TestExecutor executor;
  LiveTrackers trackers;
  MockProbe probe;
  EXPECT_CALL(probe, Run(testing::_))
      .WillRepeatedly(MakeLifetimeReportingAction(trackers));

  Awaitable<bool> pending = probe.Run(&trackers);
  EXPECT_FALSE(WaitAwaitable(executor, std::move(pending)));
}

// `ON_CALL` defaults are performed in place, on the action the spec stores, so
// the closure lives as long as the mock does. This is why the check does not
// flag `WillByDefault`.
TEST(GmockAwaitableActionLifetime, OnCallDefaultRunsAgainstALiveClosure) {
  TestExecutor executor;
  LiveTrackers trackers;
  testing::NiceMock<MockProbe> probe;
  ON_CALL(probe, Run(testing::_))
      .WillByDefault(MakeLifetimeReportingAction(trackers));

  Awaitable<bool> pending = probe.Run(&trackers);
  EXPECT_TRUE(WaitAwaitable(executor, std::move(pending)));
}

// An `EXPECT_CALL` with no action of its own falls back to the `ON_CALL`
// default, and that path does not copy either.
TEST(GmockAwaitableActionLifetime,
     ExpectCallFallingBackToDefaultRunsAgainstALiveClosure) {
  TestExecutor executor;
  LiveTrackers trackers;
  MockProbe probe;
  ON_CALL(probe, Run(testing::_))
      .WillByDefault(MakeLifetimeReportingAction(trackers));
  EXPECT_CALL(probe, Run(testing::_));

  Awaitable<bool> pending = probe.Run(&trackers);
  EXPECT_TRUE(WaitAwaitable(executor, std::move(pending)));
}

}  // namespace
}  // namespace scada
