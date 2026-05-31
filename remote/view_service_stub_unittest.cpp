#include "remote/view_service_stub.h"

#include "base/test/test_executor.h"
#include "remote/message_sender_mock.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/coroutine_services.h"

#include <boost/asio/post.hpp>
#include <boost/asio/this_coro.hpp>

#include <gmock/gmock.h>

using namespace testing;

namespace {

class TestViewService final : public scada::ViewService {
 public:
  Awaitable<scada::StatusOr<std::vector<scada::BrowseResult>>> Browse(
      scada::ServiceContext context,
      std::vector<scada::BrowseDescription> inputs) override {
    co_await boost::asio::post(co_await boost::asio::this_coro::executor,
                               boost::asio::use_awaitable);
    browse_called = true;
    last_user_id = context.user_id();
    last_browse_inputs = std::move(inputs);
    co_return std::vector<scada::BrowseResult>{
        {.status_code = scada::StatusCode::Good}};
  }

  Awaitable<scada::StatusOr<std::vector<scada::BrowsePathResult>>>
  TranslateBrowsePaths(std::vector<scada::BrowsePath>) override {
    co_return std::vector<scada::BrowsePathResult>{};
  }

  bool browse_called = false;
  scada::NodeId last_user_id;
  std::vector<scada::BrowseDescription> last_browse_inputs;
};

// Regression test for a use-after-free in ViewServiceStub's constructor:
// the class used to shadow the `coroutine_service_` reference from its
// ViewServiceStubContext base with a duplicate member, and the mem-initializer
// `coroutine_service_{coroutine_service_}` bound the derived-class reference
// to itself — leaving a dangling reference. The first Browse request would
// then crash inside the resumed OnBrowseAsync coroutine with an access
// violation while dereferencing the bogus service pointer.
//
// With the duplicate member removed, the stub resolves `coroutine_service_`
// through the base class and the Browse call reaches the real service.
TEST(ViewServiceStubTest, BrowseRoutesToCoroutineServiceFromContext) {
  TestExecutor executor;
  auto sender = std::make_shared<StrictMock<MessageSenderMock>>();
  TestViewService service;

  auto stub = std::make_shared<ViewServiceStub>(ViewServiceStubContext{
      .executor_ = executor,
      .sender_ = sender,
      .service_context_ = scada::ServiceContext{}.with_user_id({1, 1}),
      .service_ = service,
  });

  EXPECT_CALL(*sender, Send(_)).WillOnce(Invoke([](protocol::Message& message) {
    ASSERT_EQ(message.responses_size(), 1);
    const auto& response = message.responses(0);
    EXPECT_EQ(response.request_id(), 42);
    EXPECT_EQ(ConvertTo<scada::Status>(response.status()).code(),
              scada::StatusCode::Good);
  }));

  protocol::Request request;
  request.set_request_id(42);
  auto* browse_proto_node = request.mutable_browse()->add_nodes();
  Convert(scada::NodeId{2, 3}, *browse_proto_node->mutable_node_id());

  stub->OnRequestReceived(request);
  while (executor.GetTaskCount() != 0) {
    executor.Poll();
  }

  ASSERT_TRUE(service.browse_called);
  EXPECT_EQ(service.last_user_id, scada::NodeId(1, 1));
  ASSERT_EQ(service.last_browse_inputs.size(), 1u);
  EXPECT_EQ(service.last_browse_inputs[0].node_id, scada::NodeId(2, 3));
}

}  // namespace
