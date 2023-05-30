#include "remote/remote_services.h"

#include "base/logger.h"
#include "base/test/asio_test.h"
#include "base/test/network_port_generator.h"
#include "base/test/test_executor.h"
#include "core/authentication_mock.h"
#include "core/client.h"
#include "core/data_services_factory.h"
#include "core/monitored_item_service_mock.h"
#include "remote/remote_session_manager.h"

#include <gmock/gmock.h>

using namespace testing;

class Server {
 public:
  Server(AsioTest& asio, const NetworkTestEnvironment& network)
      : asio_{asio}, network_{network} {
    ON_CALL(authenticator_, Call(_, _))
        .WillByDefault(Return(make_resolved_promise(
            scada::AuthenticationResult{.user_id = {1, 1}})));

    ON_CALL(*monitored_item_service_.default_monitored_item,
            Subscribe(VariantWith<scada::DataChangeHandler>(_)))
        .WillByDefault(Invoke([](const scada::MonitoredItemHandler& handler) {
          std::get<scada::DataChangeHandler>(handler)(
              scada::MakeReadResult(123));
        }));

    asio_.Wait(session_manager_.Init());
  }

  AsioTest& asio_;
  const NetworkTestEnvironment& network_;

  NiceMock<scada::MockAuthenticator> authenticator_;

  NiceMock<scada::MockMonitoredItemService> monitored_item_service_;

  RemoteSessionManager session_manager_{
      {.executor_ = asio_.executor_,
       .services_ = {.monitored_item_service = &monitored_item_service_},
       .authenticator_ = authenticator_.AsStdFunction(),
       .transport_factory_ = asio_.transport_factory_,
       .endpoints_ = {net::TransportString{network_.server_transport_string}}}};
};

class RemoteServicesTest : public Test {
 public:
  virtual void SetUp() override;
  virtual void TearDown() override;

  AsioTest asio_;

  const NetworkTestEnvironment network_env_ = GenerateNetworkTestEnvironment();

  Server server_{asio_, network_env_};

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

  asio_.Wait(client_.connect(
      {.connection_string = network_env_.client_transport_string,
       .user_name = u"username",
       .password = u"password"}));

  connected_ = true;
}

void RemoteServicesTest::TearDown() {
  if (connected_) {
    asio_.Wait(client_.disconnect());
  }
}

TEST_F(RemoteServicesTest, Test) {
  const scada::NodeId node_id{1, 1};

  promise<scada::DataValue> promise;
  MockFunction<void(const scada::DataValue& data_value)> data_change_handler;

  EXPECT_CALL(data_change_handler, Call(_))
      .WillOnce(Invoke([promise](const scada::DataValue& data_value) mutable {
        promise.resolve(data_value);
      }));

  auto monitored_item = client_.node(node_id).subscribe_value(
      data_change_handler.AsStdFunction());

  asio_.Wait(promise);
}
