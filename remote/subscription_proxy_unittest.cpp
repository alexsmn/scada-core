#include "remote/subscription_proxy.h"

#include "remote/message_sender_fake.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "remote/subscription.h"

#include <memory>

#include <gmock/gmock.h>

using namespace testing;

namespace {

// `SessionStub` decides what an incoming request *is* by which optional
// message field is set — `has_create_subscription()` — and never by a request
// id or a type tag. So the create-subscription request has to carry that
// field even though it has no payload to put in it: the field's presence is
// the whole message.
//
// That makes `request.mutable_create_subscription()` a call whose only
// purpose is its side effect, which reads as dead code at the call site and
// invites exactly the deletion this test refuses. (Task 312: the line used to
// be followed by a bare `create_subscription;` statement, added to silence an
// unused-variable warning, and cppcheck flagged that statement rather than
// the hazard underneath it.)
TEST(SubscriptionProxyTest, OpeningTheChannelRequestsCreateSubscription) {
  auto proxy = std::make_shared<SubscriptionProxy>(SubscriptionParams{});
  MessageSenderFake sender;

  proxy->OnChannelOpened(sender);

  ASSERT_THAT(sender.requests(), SizeIs(1));
  EXPECT_TRUE(sender.requests().front().request.has_create_subscription());
}

// A failed CreateSubscription leaves the channel open with no subscription, and
// closing that channel afterwards is ordinary. It panicked: the failed result
// set the state to DELETED while the channel stayed set, and OnChannelClosed()
// checked the state as well. A session closing while CreateSubscription is in
// flight does exactly this -- it fails the pending request with
// Bad_Disconnected and then closes the channel -- so a server restart at that
// moment crashed the client (found under backlog 922).
TEST(SubscriptionProxyTest, ClosingAfterAFailedCreateIsOrdinary) {
  auto proxy = std::make_shared<SubscriptionProxy>(SubscriptionParams{});
  MessageSenderFake sender;
  proxy->OnChannelOpened(sender);
  ASSERT_THAT(sender.requests(), SizeIs(1));

  protocol::Response response;
  response.set_request_id(1);
  Convert(scada::Status{scada::StatusCode::Bad_Disconnected},
          *response.mutable_status());
  sender.requests().front().response_handler(response);

  proxy->OnChannelClosed();

  // And the channel can open again, as it does on reconnect.
  proxy->OnChannelOpened(sender);
  EXPECT_THAT(sender.requests(), SizeIs(2));
}

}  // namespace
