#include "scada/authentication_adapters.h"

#include "base/test/awaitable_test.h"
#include "scada/co_result.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

class StubCoroutineAuthenticator final : public CoroutineAuthenticator {
 public:
  explicit StubCoroutineAuthenticator(StatusOr<AuthenticationResult> result)
      : result_{std::move(result)} {}

  CoStatusOr<AuthenticationResult> Authenticate(
      LocalizedText user_name,
      LocalizedText password) override {
    EXPECT_EQ(user_name, LocalizedText{u"user"});
    EXPECT_EQ(password, LocalizedText{u"password"});
    co_return result_;
  }

 private:
  StatusOr<AuthenticationResult> result_;
};

TEST(AuthenticationAdaptersTest, AuthenticatorIsCoroutineFunction) {
  TestExecutor executor{true};

  auto authenticator = MakeAuthenticator(
      executor,
      [](LocalizedText user_name,
         LocalizedText password) -> CoStatusOr<AuthenticationResult> {
        EXPECT_EQ(user_name, LocalizedText{u"user"});
        EXPECT_EQ(password, LocalizedText{u"password"});
        co_return AuthenticationResult{
            .user_id = NodeId{1, 2}, .user_rights = 7, .multi_sessions = true};
      });

  auto result = WaitAwaitable(executor, authenticator(u"user", u"password"));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(result->user_id, (NodeId{1, 2}));
  EXPECT_EQ(result->user_rights, 7u);
  EXPECT_TRUE(result->multi_sessions);
}

TEST(AuthenticationAdaptersTest, AuthenticatorPreservesBadStatusResult) {
  TestExecutor executor{true};

  auto authenticator = MakeAuthenticator(
      executor,
      [](LocalizedText, LocalizedText) -> CoStatusOr<AuthenticationResult> {
        co_return StatusCode::Bad_WrongLoginCredentials;
      });

  auto result = WaitAwaitable(executor, authenticator(u"user", u"password"));

  ASSERT_FALSE(result.ok());
  EXPECT_EQ(result.status().code(), StatusCode::Bad_WrongLoginCredentials);
}

TEST(AuthenticationAdaptersTest,
     MakeAuthenticatorAdaptsCoroutineAuthenticatorInterface) {
  TestExecutor executor{true};
  StubCoroutineAuthenticator authenticator{AuthenticationResult{
      .user_id = NodeId{5, 6}, .user_rights = 11, .multi_sessions = true}};

  auto function_authenticator = MakeAuthenticator(executor, authenticator);

  auto result =
      WaitAwaitable(executor, function_authenticator(u"user", u"password"));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(result->user_id, (NodeId{5, 6}));
  EXPECT_EQ(result->user_rights, 11u);
  EXPECT_TRUE(result->multi_sessions);
}

TEST(AuthenticationAdaptersTest, MakeCoroutineAuthenticatorWrapsFunction) {
  TestExecutor executor{true};
  auto coroutine_authenticator = MakeCoroutineAuthenticator(
      executor,
      [](LocalizedText, LocalizedText) -> CoStatusOr<AuthenticationResult> {
        co_return AuthenticationResult{.user_id = NodeId{7, 8},
                                       .user_rights = 13,
                                       .multi_sessions = false};
      });

  auto result = WaitAwaitable(
      executor, coroutine_authenticator->Authenticate(u"user", u"password"));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(result->user_id, (NodeId{7, 8}));
  EXPECT_EQ(result->user_rights, 13u);
  EXPECT_FALSE(result->multi_sessions);
}

}  // namespace
}  // namespace scada
