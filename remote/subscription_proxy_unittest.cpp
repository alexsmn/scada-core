#include "remote/subscription_proxy.h"

#include "remote/message_sender_fake.h"
#include "remote/protocol.h"
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

}  // namespace
