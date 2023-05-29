#include "remote/remote_services.h"
#include "base/logger.h"
#include "base/test/asio_test.h"
#include "base/test/test_executor.h"
#include "core/client.h"
#include "core/data_services_factory.h"

#include <gmock/gmock.h>

using namespace testing;

class RemoteServicesTest : public Test {
 public:
  virtual void SetUp() override;
  virtual void TearDown() override;

  AsioTest asio_;

  DataServices data_services_;

  scada::client client_;

  bool connected_ = false;
};

void RemoteServicesTest::SetUp() {
  ASSERT_TRUE(CreateRemoteServices(
      DataServicesContext{.logger = NullLogger::GetInstance(),
                          .executor = std::make_shared<TestExecutor>(),
                          .transport_factory = asio_.transport_factory_},
      data_services_));

  client_ = scada::client{
      {.monitored_item_service = data_services_.monitored_item_service_.get(),
       .session_service = data_services_.session_service_.get()}};

  asio_.Wait(
      client_.connect({.user_name = u"username", .password = u"password"}));

  connected_ = true;
}

void RemoteServicesTest::TearDown() {
  if (connected_) {
    asio_.Wait(client_.disconnect());
  }
}

TEST_F(RemoteServicesTest, Test) {
  const scada::NodeId node_id{1, 1};

  MockFunction<void(const scada::DataValue& data_value)> data_change_handler;

  auto monitored_item = client_.node(node_id).subscribe_value(
      data_change_handler.AsStdFunction());

  EXPECT_CALL(data_change_handler, Call(_));
}
