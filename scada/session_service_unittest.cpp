#include "scada/session_service.h"

#include <gtest/gtest.h>

namespace scada {

namespace {

// A minimal SessionService that answers only the access-rights question the
// permission accessors are derived from. It deliberately does NOT override
// IsAnonymous(), so it also exercises the interface's default.
class TestSessionService : public SessionService {
 public:
  explicit TestSessionService(std::uint32_t access_rights)
      : access_rights_{access_rights} {}

  Awaitable<void> Connect(SessionConnectParams /*params*/) override {
    co_return;
  }
  Awaitable<void> Reconnect() override { co_return; }
  Awaitable<void> Disconnect() override { co_return; }
  bool IsConnected(scada::Duration* /*ping_delay*/) const override {
    return true;
  }
  NodeId GetUserId() const override { return {}; }
  std::uint32_t GetAccessRights() const override { return access_rights_; }
  std::string GetHostName() const override { return "test"; }
  bool IsScada() const override { return true; }
  boost::signals2::scoped_connection SubscribeSessionStateChanged(
      const SessionStateChangedCallback& /*callback*/) override {
    return {};
  }
  SessionDebugger* GetSessionDebugger() override { return nullptr; }

 private:
  const std::uint32_t access_rights_;
};

class AnonymousSessionService final : public TestSessionService {
 public:
  AnonymousSessionService() : TestSessionService{0} {}

  bool IsAnonymous() const override { return true; }
};

constexpr std::uint32_t kConfigure = AccessRightBit(AccessRight::kConfigure);
constexpr std::uint32_t kControl = AccessRightBit(AccessRight::kControl);

TEST(SessionServiceTest, AccessRightsAreReadOffTheBitmask) {
  const TestSessionService control{kControl};
  EXPECT_TRUE(control.HasAccessRight(AccessRight::kControl));
  EXPECT_FALSE(control.HasAccessRight(AccessRight::kConfigure));

  const TestSessionService none{0};
  EXPECT_FALSE(none.HasAccessRight(AccessRight::kControl));
  EXPECT_FALSE(none.HasAccessRight(AccessRight::kConfigure));
}

TEST(SessionServiceTest, PermissionsMatchTheServerDerivation) {
  // The accessors must agree with the derivation the server applies to the
  // same bitmask, or a client-side gate and the server's enforcement disagree.
  const TestSessionService session{kControl};

  EXPECT_EQ(session.GetPermissions(),
            PermissionsForUser(kControl, /*is_anonymous=*/false));
  EXPECT_EQ(session.HasPermission(Permission::kWrite),
            IsPermitted(kControl, false, Permission::kWrite));
}

TEST(SessionServiceTest, ControlGrantsWriteAndCallButNotNodeManagement) {
  const TestSessionService session{kControl};

  EXPECT_TRUE(session.HasPermission(Permission::kRead));
  EXPECT_TRUE(session.HasPermission(Permission::kWrite));
  EXPECT_TRUE(session.HasPermission(Permission::kCall));
  EXPECT_FALSE(session.HasPermission(Permission::kAddNode));
  EXPECT_FALSE(session.HasPermission(Permission::kDeleteNode));
}

TEST(SessionServiceTest, ConfigureGrantsNodeManagement) {
  const TestSessionService session{kConfigure};

  EXPECT_TRUE(session.HasPermission(Permission::kAddNode));
  EXPECT_TRUE(session.HasPermission(Permission::kDeleteNode));
  // An administrator may also write and call: the client gates must not be
  // stricter than the server, which grants the admin roles the full set.
  EXPECT_TRUE(session.HasPermission(Permission::kWrite));
  EXPECT_TRUE(session.HasPermission(Permission::kCall));
}

TEST(SessionServiceTest, ObserverMayOnlyRead) {
  const TestSessionService session{0};

  EXPECT_TRUE(session.HasPermission(Permission::kBrowse));
  EXPECT_TRUE(session.HasPermission(Permission::kRead));
  EXPECT_TRUE(session.HasPermission(Permission::kReadHistory));
  EXPECT_FALSE(session.HasPermission(Permission::kWrite));
  EXPECT_FALSE(session.HasPermission(Permission::kCall));
  EXPECT_FALSE(session.HasPermission(Permission::kAddNode));
}

TEST(SessionServiceTest, SessionsAreAuthenticatedUnlessTheySayOtherwise) {
  // The default matters: an in-process or vendor-bridge session carries no
  // user id, and treating it as anonymous would silently demote it.
  const TestSessionService session{0};
  EXPECT_FALSE(session.IsAnonymous());
}

TEST(SessionServiceTest, AnonymousLosesTheAuthenticatedBaseline) {
  const AnonymousSessionService session;

  EXPECT_TRUE(session.IsAnonymous());
  EXPECT_TRUE(session.HasPermission(Permission::kRead));
  EXPECT_FALSE(session.HasPermission(Permission::kReadHistory));
  EXPECT_FALSE(session.HasPermission(Permission::kWrite));
}

TEST(SessionServiceTest, MultiBitPermissionRequiresEveryBit) {
  const TestSessionService operator_session{kControl};

  EXPECT_TRUE(
      operator_session.HasPermission(Permission::kRead | Permission::kWrite));
  EXPECT_FALSE(operator_session.HasPermission(Permission::kWrite |
                                              Permission::kAddNode));
}

}  // namespace

}  // namespace scada
