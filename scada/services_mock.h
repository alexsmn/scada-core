#pragma once

#include "scada/monitored_item_service_mock.h"
#include "scada/session_service_mock.h"

namespace scada {

struct MockServices {
  MockServices() {
    using namespace testing;

    ON_CALL(session_service, HasPrivilege(_)).WillByDefault(Return(true));

    ON_CALL(session_service, Disconnect())
        .WillByDefault(Return(make_resolved_promise()));
  }

  services services() {
    return {.monitored_item_service = &monitored_item_service,
            .session_service = &session_service};
  }

  testing::NiceMock<MockMonitoredItemService> monitored_item_service;
  testing::NiceMock<MockSessionService> session_service;
};

}  // namespace scada