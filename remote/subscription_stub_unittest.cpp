#include "remote/subscription_stub.h"

#include "base/test/test_executor.h"
#include "remote/message_sender_mock.h"
#include "remote/protocol.h"
#include "scada/event.h"
#include "scada/item_factory_subscription.h"
#include "scada/monitored_item_service.h"
#include "scada/monitoring_parameters.h"
#include "scada/read_value_id.h"
#include "scada/test/test_monitored_item.h"

#include <any>
#include <memory>

#include <gmock/gmock.h>

using namespace testing;

namespace {

// Hands every requested item back as a TestMonitoredItem the test can fire
// notifications from, mirroring how EventMonitoredItemService builds its
// subscription (see item_factory_subscription.h).
class FactoryMonitoredItemService final : public scada::MonitoredItemService {
 public:
  scada::StatusOr<std::unique_ptr<scada::MonitoredItemSubscription>>
  CreateSubscription(scada::ServiceContext,
                     scada::MonitoredItemSubscriptionOptions options) override {
    return scada::MakeItemFactorySubscription(
        [this](const scada::ReadValueId&, const scada::MonitoringParameters&) {
          item = std::make_shared<scada::TestMonitoredItem>();
          return item;
        },
        options);
  }

  std::shared_ptr<scada::TestMonitoredItem> item;
};

class SubscriptionStubEventTest : public Test {
 protected:
  // The notification path is a coroutine chain (LegacyMonitoredItemAdapter
  // reads the subscription's stream, which the item factory feeds), so each
  // continuation posts a fresh task. Run to quiescence, not just one round.
  void Drain() {
    for (int i = 0; i < 100 && executor_.HasReadyTasks(); ++i) {
      executor_.Poll();
    }
  }

  // Creates the stub's single EventNotifier item and runs the executor until
  // the subscription plumbing has handed the factory item its handler.
  void CreateEventItem() {
    stub_->OnCreateMonitoredItem(
        /*request_id=*/1,
        scada::ReadValueId{.node_id = scada::NodeId{42u},
                           .attribute_id = scada::AttributeId::EventNotifier},
        scada::MonitoringParameters{});
    Drain();
    ASSERT_NE(service_.item, nullptr);
    ASSERT_TRUE(service_.item->subscribed());
  }

  void FireEvent(const scada::Event& event) {
    service_.item->NotifyEvent(std::any{event});
    Drain();
  }

  static scada::Event MakeValidEvent() {
    scada::Event event;
    event.event_id = 0x2a;
    event.time = scada::Now();
    event.receive_time = event.time;
    event.message = scada::LocalizedText{u"pump tripped"};
    return event;
  }

  TestExecutor executor_;
  FactoryMonitoredItemService service_;
  std::shared_ptr<MessageSenderMock> sender_ =
      std::make_shared<MessageSenderMock>();
  std::shared_ptr<SubscriptionStub> stub_ =
      std::make_shared<SubscriptionStub>(executor_,
                                         sender_,
                                         service_,
                                         /*subscription_id=*/1,
                                         SubscriptionParams{});
};

// An event that crossed a tier boundary is external input and must never panic
// this process. protocol_utils' Convert() requires a non-zero event id (OPC UA
// Part 5 §6.4.2 BaseEventType,
// https://reference.opcfoundation.org/Core/Part5/v105/docs/6.4.2) and panics
// without one; the aggregating proxy used to relay exactly such an event —
// reassembled from a downstream tier's payload-less notification — and died.
TEST_F(SubscriptionStubEventTest, DropsEventWithoutEventId) {
  ASSERT_NO_FATAL_FAILURE(CreateEventItem());
  EXPECT_CALL(*sender_, Send(_)).Times(0);

  scada::Event event = MakeValidEvent();
  event.event_id = 0;
  FireEvent(event);
}

TEST_F(SubscriptionStubEventTest, SendsEventWithEventId) {
  ASSERT_NO_FATAL_FAILURE(CreateEventItem());
  protocol::Message sent;
  EXPECT_CALL(*sender_, Send(_)).WillOnce(SaveArg<0>(&sent));

  FireEvent(MakeValidEvent());

  ASSERT_EQ(sent.notifications_size(), 1);
  ASSERT_EQ(sent.notifications(0).events_size(), 1);
  EXPECT_EQ(sent.notifications(0).events(0).event_id(), 0x2au);
}

}  // namespace
