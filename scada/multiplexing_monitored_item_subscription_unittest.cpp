#include "scada/multiplexing_monitored_item_subscription.h"

#include "base/check.h"
#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/co_result.h"
#include "scada/monitored_item_service.h"
#include "scada/serving_gate.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <deque>
#include <future>
#include <memory>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

namespace scada {
namespace {

using namespace testing;

// --- ServingGate (the shared block/allow gate) ------------------------------

TEST(ServingGate, AllowedByDefault) {
  ServingGate gate;
  EXPECT_TRUE(gate.allowed());
}

TEST(ServingGate, BlockDisallowsAndSweepsRegisteredSubscriptions) {
  ServingGate gate;
  std::vector<StatusCode> swept;
  auto registration =
      gate.Register([&](StatusCode status) { swept.push_back(status); });

  gate.SetBlocked(StatusCode::Bad_LicenseExpired);

  EXPECT_FALSE(gate.allowed());
  EXPECT_EQ(gate.blocked_status(), StatusCode::Bad_LicenseExpired);
  EXPECT_THAT(swept, ElementsAre(StatusCode::Bad_LicenseExpired));

  gate.SetAllowed();
  EXPECT_TRUE(gate.allowed());
}

TEST(ServingGate, DroppedRegistrationIsNotSwept) {
  ServingGate gate;
  int calls = 0;
  {
    auto registration = gate.Register([&](StatusCode) { ++calls; });
  }

  gate.SetBlocked(StatusCode::Bad_LicenseExpired);

  EXPECT_EQ(calls, 0);
}

// Regression: a Registration that outlives its ServingGate must destroy
// cleanly. Previously the Registration held a raw ServingGate* and unregistered
// from the gate's mutex in its destructor, so a subscription State torn down
// after the RootNodeManager that owns the gate locked a destroyed std::mutex
// and aborted with "mutex lock failed: Invalid argument". The registry is now a
// shared control block referenced weakly, so the late unregister is a no-op.
TEST(ServingGate, RegistrationOutlivingGateDestroysSafely) {
  ServingGate::Registration registration;
  {
    ServingGate gate;
    registration = gate.Register([](StatusCode) {});
  }
  // `registration` destroyed here, after `gate`: must not touch the destroyed
  // gate. Reaching this point without aborting is the assertion.
  SUCCEED();
}

// A View is a strong handle to the shared block/allow state, so it stays
// readable after the gate is destroyed (previously the subscription held a raw
// ServingGate* and read it at notification time — a use-after-free once the
// subscription outlived the gate). The View freezes on the last published
// state.
TEST(ServingGate, ViewOutlivingGateReadsSafely) {
  ServingGate::View view;
  {
    ServingGate gate;
    gate.SetBlocked(StatusCode::Bad_LicenseExpired);
    view = gate.view();
  }
  // Reads the last published state through the destroyed gate's shared block.
  EXPECT_FALSE(view.allowed());
  EXPECT_EQ(view.blocked_status(), StatusCode::Bad_LicenseExpired);
}

// A default-constructed View means "no gate": always allowed.
TEST(ServingGate, DefaultViewIsAllowed) {
  ServingGate::View view;
  EXPECT_TRUE(view.allowed());
  EXPECT_EQ(view.blocked_status(), StatusCode::Good);
}

// --- Multiplexing subscription honoring the gate ----------------------------

// A backend subscription seeded with fixed add/read results, mirroring the pump
// unit test's fake so this test can drive item creation without a real backend.
// Like a live session, it waits for data rather than ending when it has none
// queued; `FakeBackendService::Fail` ends it, the way a dropped downstream
// session closes every subscription it carried.
class FakeBackendSubscription final : public MonitoredItemSubscription {
 public:
  struct State {
    std::vector<MonitoredItemCreateResult> add_results;
    std::deque<StatusOr<std::vector<MonitoredItemNotification>>> read_results;
    // Every request any subscription of this backend was asked to add.
    std::vector<MonitoredItemCreateRequest> added;
    // Wakes a waiting ReadNext when data is queued or the backend fails.
    std::vector<std::weak_ptr<boost::asio::steady_timer>> waiters;
    // Bumped by Fail: a subscription created before it reads `failure`.
    int generation = 0;
    Status failure = StatusCode::Good;

    void Wake() {
      for (const auto& weak_waiter : waiters) {
        if (auto waiter = weak_waiter.lock()) {
          waiter->cancel();
        }
      }
    }
  };

  explicit FakeBackendSubscription(std::shared_ptr<State> state)
      : state_{std::move(state)}, generation_{state_->generation} {}

  Awaitable<std::vector<MonitoredItemCreateResult>> AddItems(
      std::vector<MonitoredItemCreateRequest> requests) override {
    state_->added.insert(state_->added.end(), requests.begin(), requests.end());
    co_return state_->add_results;
  }

  Awaitable<std::vector<Status>> RemoveItems(
      std::span<const MonitoredItemId> item_ids) override {
    co_return std::vector<Status>(item_ids.size(), Status{StatusCode::Good});
  }

  CoStatusOr<std::vector<MonitoredItemNotification>> ReadNext(
      std::size_t) override {
    for (;;) {
      if (closed_) {
        co_return Status{StatusCode::Bad_Disconnected};
      }
      if (generation_ != state_->generation) {
        co_return state_->failure;
      }
      if (!state_->read_results.empty()) {
        StatusOr<std::vector<MonitoredItemNotification>> result =
            std::move(state_->read_results.front());
        state_->read_results.pop_front();
        co_return result;
      }
      waiter_ = std::make_shared<boost::asio::steady_timer>(
          co_await boost::asio::this_coro::executor);
      waiter_->expires_at((boost::asio::steady_timer::time_point::max)());
      state_->waiters.push_back(waiter_);
      boost::system::error_code error;
      co_await waiter_->async_wait(
          boost::asio::redirect_error(boost::asio::use_awaitable, error));
    }
  }

  void Close(Status) override {
    closed_ = true;
    if (waiter_) {
      waiter_->cancel();
    }
  }

 private:
  std::shared_ptr<State> state_;
  const int generation_;
  bool closed_ = false;
  std::shared_ptr<boost::asio::steady_timer> waiter_;
};

class FakeBackendService final : public MonitoredItemService {
 public:
  explicit FakeBackendService(
      std::shared_ptr<FakeBackendSubscription::State> state)
      : state_{std::move(state)} {}

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> CreateSubscription(
      ServiceContext,
      MonitoredItemSubscriptionOptions) override {
    return std::unique_ptr<MonitoredItemSubscription>{
        std::make_unique<FakeBackendSubscription>(state_)};
  }

  // Ends every subscription this backend has handed out with `status`.
  void Fail(Status status) {
    state_->failure = std::move(status);
    ++state_->generation;
    state_->Wake();
  }

  // Queues a data change for the subscription now reading.
  void Deliver(MonitoredItemId router_item_id, int value) {
    state_->read_results.push_back(
        std::vector<MonitoredItemNotification>{DataChangeNotification{
            .item_id = router_item_id,
            .client_handle = static_cast<std::uint32_t>(router_item_id),
            .value = DataValue{Variant{value}, Qualifier{}, Time{}, Time{}}}});
    state_->Wake();
  }

 private:
  std::shared_ptr<FakeBackendSubscription::State> state_;
};

// When the gate blocks after an item is already being monitored, the item is
// swept to the blocked status so a client sees the outage even if its backend
// value never changes again.
TEST(MultiplexingSubscriptionServingGate, SweepsActiveItemOnBlock) {
  TestExecutor executor;
  ServingGate gate;

  auto backend_state = std::make_shared<FakeBackendSubscription::State>();
  backend_state->add_results.push_back(
      {.item_id = 1, .client_handle = 0, .status = StatusCode::Good});
  FakeBackendService service{backend_state};

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> subscription =
      MakeMultiplexingMonitoredItemSubscription(
          executor,
          [&](const ReadValueId& value_id, const MonitoringParameters&) {
            return std::vector<MonitoredItemRoute>{
                {.service = &service, .value_id = value_id}};
          },
          {}, &gate);
  ASSERT_TRUE(subscription.ok());

  std::vector<MonitoredItemCreateRequest> requests{
      {.item_to_monitor = {.node_id = NodeId{1, 1}}, .client_handle = 77}};
  std::vector<MonitoredItemCreateResult> add_results =
      WaitAwaitable(executor, (*subscription)->AddItems(std::move(requests)));
  Drain(executor);
  ASSERT_THAT(add_results, SizeIs(1));
  ASSERT_EQ(add_results[0].status, Status{StatusCode::Good});
  const MonitoredItemId router_item_id = add_results[0].item_id;

  // Runtime license expiry blocks the gate.
  gate.SetBlocked(StatusCode::Bad_LicenseExpired);

  StatusOr<std::vector<MonitoredItemNotification>> notifications =
      WaitAwaitable(executor, (*subscription)->ReadNext(10));
  ASSERT_TRUE(notifications.ok());
  ASSERT_THAT(*notifications, SizeIs(1));
  const auto* status_note =
      std::get_if<ItemStatusNotification>(&notifications->front());
  ASSERT_NE(status_note, nullptr);
  EXPECT_EQ(status_note->item_id, router_item_id);
  EXPECT_EQ(status_note->client_handle, 77u);
  EXPECT_EQ(status_note->status.code(), StatusCode::Bad_LicenseExpired);
}

// A backend sample delivered while the gate is blocked is rewritten to the
// blocked status, so a live value does not leak through during the outage.
TEST(MultiplexingSubscriptionServingGate, RewritesBackendSampleWhileBlocked) {
  TestExecutor executor;
  ServingGate gate;
  gate.SetBlocked(StatusCode::Bad_LicenseExpired);

  auto backend_state = std::make_shared<FakeBackendSubscription::State>();
  backend_state->add_results.push_back(
      {.item_id = 1, .client_handle = 0, .status = StatusCode::Good});
  // The backend delivers one good data-change; `client_handle` carries the
  // router item id the multiplexing layer assigned (the first item -> 1).
  backend_state->read_results.push_back(
      std::vector<MonitoredItemNotification>{DataChangeNotification{
          .item_id = 1,
          .client_handle = 1,
          .value = DataValue{Variant{42}, Qualifier{}, Time{}, Time{}}}});
  FakeBackendService service{backend_state};

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> subscription =
      MakeMultiplexingMonitoredItemSubscription(
          executor,
          [&](const ReadValueId& value_id, const MonitoringParameters&) {
            return std::vector<MonitoredItemRoute>{
                {.service = &service, .value_id = value_id}};
          },
          {}, &gate);
  ASSERT_TRUE(subscription.ok());

  std::vector<MonitoredItemCreateRequest> requests{
      {.item_to_monitor = {.node_id = NodeId{1, 1}}, .client_handle = 77}};
  WaitAwaitable(executor, (*subscription)->AddItems(std::move(requests)));
  Drain(executor);

  StatusOr<std::vector<MonitoredItemNotification>> notifications =
      WaitAwaitable(executor, (*subscription)->ReadNext(10));
  ASSERT_TRUE(notifications.ok());
  const DataChangeNotification* data_change = nullptr;
  for (const auto& notification : *notifications) {
    if (const auto* candidate =
            std::get_if<DataChangeNotification>(&notification)) {
      data_change = candidate;
    }
  }
  ASSERT_NE(data_change, nullptr);
  EXPECT_EQ(data_change->value.status_code, StatusCode::Bad_LicenseExpired);
}

// --- Re-routing items whose backend went away (backlog 854) ----------------

// Small enough that a test waits milliseconds, not the production second.
constexpr std::chrono::milliseconds kTestRebindDelay{1};

// Reads notifications until one satisfies `match`, or gives up after a few
// seconds of real time (the rebind wait runs on a real timer). Returns the
// matching notification.
template <class Match>
std::optional<MonitoredItemNotification> ReadUntil(
    TestExecutor& executor,
    MonitoredItemSubscription& subscription,
    Match match) {
  const auto deadline = std::chrono::steady_clock::now() + 5s;
  while (std::chrono::steady_clock::now() < deadline) {
    auto result = StartAwaitable(executor, subscription.ReadNext(10));
    while (!result->done && std::chrono::steady_clock::now() < deadline) {
      Drain(executor);
      std::this_thread::yield();
    }
    if (!result->done || !result->value || !result->value->ok()) {
      return std::nullopt;
    }
    for (auto& notification : **result->value) {
      if (match(notification)) {
        return std::move(notification);
      }
    }
  }
  return std::nullopt;
}

bool IsValue(const MonitoredItemNotification& notification, int value) {
  const auto* data_change = std::get_if<DataChangeNotification>(&notification);
  return data_change && data_change->value.value == Variant{value};
}

std::shared_ptr<FakeBackendSubscription::State> AcceptingBackend() {
  auto state = std::make_shared<FakeBackendSubscription::State>();
  state->add_results.push_back(
      {.item_id = 1, .client_handle = 0, .status = StatusCode::Good});
  return state;
}

MonitoredItemId AddOneItem(TestExecutor& executor,
                           MonitoredItemSubscription& subscription) {
  std::vector<MonitoredItemCreateRequest> requests{
      {.item_to_monitor = {.node_id = NodeId{1, 1}}, .client_handle = 77}};
  std::vector<MonitoredItemCreateResult> results =
      WaitAwaitable(executor, subscription.AddItems(std::move(requests)));
  Drain(executor);
  base::Check(results.size() == 1 && results[0].status.good(),
              "AddOneItem: the item was refused");
  return results[0].item_id;
}

// The edge-restart case: the backend's session drops and comes back with the
// same service. The client is told about the outage, and values resume without
// the client re-adding anything.
TEST(MultiplexingSubscriptionRebind, ResumesOnTheSameBackendAfterItFails) {
  TestExecutor executor;
  auto backend_state = AcceptingBackend();
  FakeBackendService service{backend_state};

  auto subscription = MakeMultiplexingMonitoredItemSubscription(
      executor,
      [&](const ReadValueId& value_id, const MonitoringParameters&) {
        return std::vector<MonitoredItemRoute>{
            {.service = &service, .value_id = value_id}};
      },
      {}, nullptr, {}, kTestRebindDelay);
  ASSERT_TRUE(subscription.ok());
  const MonitoredItemId item_id = AddOneItem(executor, **subscription);

  service.Fail(StatusCode::Bad_Disconnected);
  auto outage = ReadUntil(executor, **subscription, [](const auto& n) {
    return std::holds_alternative<ItemStatusNotification>(n);
  });
  ASSERT_TRUE(outage.has_value());
  const auto& status_note = std::get<ItemStatusNotification>(*outage);
  EXPECT_EQ(status_note.item_id, item_id);
  EXPECT_EQ(status_note.client_handle, 77u);
  EXPECT_EQ(status_note.status.code(), StatusCode::Bad_Disconnected);

  service.Deliver(item_id, 5);
  auto resumed = ReadUntil(executor, **subscription,
                           [](const auto& n) { return IsValue(n, 5); });
  ASSERT_TRUE(resumed.has_value());
  EXPECT_EQ(std::get<DataChangeNotification>(*resumed).client_handle, 77u);
  // Added once at creation and once more when it was routed again.
  EXPECT_THAT(backend_state->added, SizeIs(2));
}

// The dynamic-downstream case: the tier that served the item is gone and its
// replacement is a different service. Until the replacement is routable the
// router finds nothing, and the item keeps waiting rather than failing.
TEST(MultiplexingSubscriptionRebind, MovesToTheBackendThatNowOwnsTheNode) {
  TestExecutor executor;
  auto old_state = AcceptingBackend();
  auto new_state = AcceptingBackend();
  FakeBackendService old_service{old_state};
  FakeBackendService new_service{new_state};
  MonitoredItemService* owner = &old_service;
  int unroutable_lookups = 0;

  auto subscription = MakeMultiplexingMonitoredItemSubscription(
      executor,
      [&](const ReadValueId& value_id, const MonitoringParameters&) {
        std::vector<MonitoredItemRoute> routes;
        if (owner) {
          routes.push_back({.service = owner, .value_id = value_id});
        } else if (++unroutable_lookups >= 3) {
          owner = &new_service;
        }
        return routes;
      },
      {}, nullptr, {}, kTestRebindDelay);
  ASSERT_TRUE(subscription.ok());
  const MonitoredItemId item_id = AddOneItem(executor, **subscription);

  owner = nullptr;
  old_service.Fail(StatusCode::Bad_Disconnected);
  new_service.Deliver(item_id, 9);
  auto resumed = ReadUntil(executor, **subscription,
                           [](const auto& n) { return IsValue(n, 9); });
  ASSERT_TRUE(resumed.has_value());
  EXPECT_GE(unroutable_lookups, 3);
  EXPECT_THAT(old_state->added, SizeIs(1));
  EXPECT_THAT(new_state->added, SizeIs(1));
}

// An item the client removed while its backend was down is not routed again.
TEST(MultiplexingSubscriptionRebind, RemovedItemIsNotRoutedAgain) {
  TestExecutor executor;
  auto backend_state = AcceptingBackend();
  FakeBackendService service{backend_state};
  int lookups = 0;

  auto subscription = MakeMultiplexingMonitoredItemSubscription(
      executor,
      [&](const ReadValueId& value_id, const MonitoringParameters&) {
        ++lookups;
        return std::vector<MonitoredItemRoute>{
            {.service = &service, .value_id = value_id}};
      },
      {}, nullptr, {}, std::chrono::milliseconds{50});
  ASSERT_TRUE(subscription.ok());
  const MonitoredItemId item_id = AddOneItem(executor, **subscription);

  service.Fail(StatusCode::Bad_Disconnected);
  Drain(executor);
  const MonitoredItemId ids[] = {item_id};
  std::vector<Status> removed =
      WaitAwaitable(executor, (*subscription)->RemoveItems(ids));
  ASSERT_THAT(removed, ElementsAre(Status{StatusCode::Good}));

  const auto until = std::chrono::steady_clock::now() + 200ms;
  while (std::chrono::steady_clock::now() < until) {
    Drain(executor);
    std::this_thread::yield();
  }
  EXPECT_EQ(lookups, 1);
  EXPECT_THAT(backend_state->added, SizeIs(1));
}

// The same edge restart on a real io_context, which is what the proxy runs.
// There co_spawn starts a coroutine inline, so a rebind started with the
// subscription's mutex held deadlocked on its first lock -- the proxy froze the
// first time an edge restarted under the 854 change. TestExecutor always
// queues and cannot show it. A deadlock here would block the io thread for
// good, so the test waits on it with a deadline and leaks the fixture rather
// than hang the binary.
TEST(MultiplexingSubscriptionRebind, ResumesOnAnIoContext) {
  struct Fixture {
    boost::asio::io_context io;
    std::shared_ptr<FakeBackendSubscription::State> backend_state =
        AcceptingBackend();
    FakeBackendService service{backend_state};
    std::promise<bool> done;
  };
  auto* fixture = new Fixture;

  boost::asio::co_spawn(
      fixture->io,
      [fixture]() -> Awaitable<void> {
        auto subscription = MakeMultiplexingMonitoredItemSubscription(
            fixture->io.get_executor(),
            [fixture](const ReadValueId& value_id,
                      const MonitoringParameters&) {
              return std::vector<MonitoredItemRoute>{
                  {.service = &fixture->service, .value_id = value_id}};
            },
            {}, nullptr, {}, kTestRebindDelay);
        std::vector<MonitoredItemCreateRequest> requests{
            {.item_to_monitor = {.node_id = NodeId{1, 1}},
             .client_handle = 77}};
        auto results = co_await (*subscription)->AddItems(std::move(requests));
        const MonitoredItemId item_id = results.at(0).item_id;

        fixture->service.Fail(StatusCode::Bad_Disconnected);
        fixture->service.Deliver(item_id, 5);
        for (;;) {
          auto notifications = co_await (*subscription)->ReadNext(10);
          if (!notifications.ok()) {
            fixture->done.set_value(false);
            co_return;
          }
          for (const auto& notification : *notifications) {
            if (IsValue(notification, 5)) {
              fixture->done.set_value(true);
              co_return;
            }
          }
        }
      },
      boost::asio::detached);

  std::future<bool> resumed = fixture->done.get_future();
  std::thread io_thread{[fixture] { fixture->io.run(); }};
  if (resumed.wait_for(5s) != std::future_status::ready) {
    ADD_FAILURE() << "the subscription did not resume: deadlocked or stuck";
    io_thread.detach();  // `fixture` is leaked with it, on purpose.
    return;
  }
  EXPECT_TRUE(resumed.get());
  fixture->io.stop();
  io_thread.join();
  delete fixture;
}

}  // namespace
}  // namespace scada
