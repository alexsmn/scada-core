#pragma once

#include "scada/monitored_item_service_mock.h"
#include "scada/session_service_mock.h"

namespace scada {

inline Awaitable<void> ReturnVoidAwaitable() {
  co_return;
}

struct MockServices {
  MockServices() {
    using namespace testing;

    // A fully entitled account by default, so permission-gated code under
    // test is not silently short-circuited.
    ON_CALL(session_service, GetAccessRights())
        .WillByDefault(Return(AccessRightBit(AccessRight::kConfigure) |
                              AccessRightBit(AccessRight::kControl)));

    ON_CALL(session_service, Disconnect()).WillByDefault([] {
      return ReturnVoidAwaitable();
    });
  }

  services services() {
    return {.monitored_item_service = &monitored_item_service,
            .session_service = &session_service};
  }

  testing::NiceMock<MockMonitoredItemService> monitored_item_service;
  testing::NiceMock<MockSessionService> session_service;
};

}  // namespace scada
