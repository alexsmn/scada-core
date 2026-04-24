#include "remote/remote_services.h"

#include "base/logger.h"
#include "base/test/asio_test_environment.h"
#include "base/test/network_test_environment.h"
#include "remote/remote_session_manager.h"
#include "scada/client.h"
#include "scada/client_monitored_item.h"
#include "scada/data_services_factory.h"
#include "scada/monitored_item.h"
#include "scada/monitored_item_service.h"
#include "scada/monitoring_parameters.h"

#include <gmock/gmock.h>

using namespace testing;

namespace {

class FakeMonitoredItem final : public scada::MonitoredItem {
 public:
  void Subscribe(scada::MonitoredItemHandler handler) override {
    if (auto* data_change_handler =
            std::get_if<scada::DataChangeHandler>(&handler)) {
      (*data_change_handler)(scada::MakeReadResult(123));
    }
  }
};

class FakeMonitoredItemService final : public scada::MonitoredItemService {
 public:
  std::shared_ptr<scada::MonitoredItem> CreateMonitoredItem(
      const scada::ReadValueId&,
      const scada::MonitoringParameters&) override {
    return monitored_item_;
  }

 private:
  std::shared_ptr<scada::MonitoredItem> monitored_item_ =
      std::make_shared<FakeMonitoredItem>();
};

}  // namespace

class TestServer {
 public:
  TestServer(AsioTestEnvironment& asio_env,
             const NetworkTestEnvironment& network)
      : asio_env_{asio_env}, network_{network} {
    asio_env_.Wait(session_manager_.Init());
  }

  AsioTestEnvironment& asio_env_;
  const NetworkTestEnvironment& network_;

  FakeMonitoredItemService monitored_item_service_;

  // Use the asio executor so coroutine continuations (e.g. the authenticator)
  // run when `asio_env_.Wait` polls the io_context. An unpolled TestExecutor
  // here causes `session_manager_.Init()` to hang indefinitely after the
  // coroutine-authenticator migration.
  RemoteSessionManager session_manager_{
      {.executor_ = asio_env_.any_executor_factory(),
       .services_ = {.monitored_item_service = &monitored_item_service_},
       .authenticator_ =
           [](scada::LocalizedText, scada::LocalizedText)
               -> Awaitable<scada::StatusOr<scada::AuthenticationResult>> {
             co_return scada::AuthenticationResult{.user_id = {1, 1}};
           },
       .transport_factory_ = asio_env_.transport_factory,
       .endpoints_ = {transport::TransportString{network_.server_transport_string}}}};
};

class RemoteServicesTest : public Test {
 public:
  virtual void SetUp() override;
  virtual void TearDown() override;

  AsioTestEnvironment asio_env_;
  NetworkTestEnvironment network_env_;

  std::unique_ptr<TestServer> server_;

  DataServices data_services_;

  scada::client client_;

  bool connected_ = false;
};

void RemoteServicesTest::SetUp() {
  server_ = std::make_unique<TestServer>(asio_env_, network_env_);

  ASSERT_TRUE(CreateRemoteServices(
      DataServicesContext{.logger = NullLogger::GetInstance(),
                          .executor = asio_env_.executor,
                          .transport_factory = asio_env_.transport_factory},
      data_services_));

  client_ = scada::client{
      {.monitored_item_service = data_services_.monitored_item_service_.get(),
       .session_service = data_services_.session_service_.get()}};

  asio_env_.Wait(client_.connect(
      {.connection_string = network_env_.client_transport_string,
       .user_name = u"username",
       .password = u"password"}));

  connected_ = true;
}

void RemoteServicesTest::TearDown() {
  if (connected_) {
    asio_env_.Wait(client_.disconnect());
    connected_ = false;
  }

  client_ = {};
  data_services_ = {};
  server_.reset();
  asio_env_.Poll();
}

TEST_F(RemoteServicesTest, Test) {
  const scada::NodeId node_id{1, 1};

  promise<scada::DataValue> promise;
  MockFunction<void(const scada::DataValue& data_value)> data_change_handler;

  EXPECT_CALL(data_change_handler, Call(_))
      .WillOnce(Invoke([promise](const scada::DataValue& data_value) mutable {
        promise.resolve(data_value);
      }));

  scada::monitored_item monitored_item;
  monitored_item.subscribe_value(client_.node(node_id), /*params=*/{},
                                 data_change_handler.AsStdFunction());

  asio_env_.Wait(promise);
}
