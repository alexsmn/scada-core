#include "scada/authentication_adapters.h"

#include "base/test/awaitable_test.h"
#include "scada/status_promise.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

class StubCoroutineAuthenticator final : public CoroutineAuthenticator {
 public:
  explicit StubCoroutineAuthenticator(StatusOr<AuthenticationResult> result)
      : result_{std::move(result)} {}

  Awaitable<StatusOr<AuthenticationResult>> Authenticate(
      LocalizedText user_name,
      LocalizedText password) override {
    EXPECT_EQ(user_name, LocalizedText{u"user"});
    EXPECT_EQ(password, LocalizedText{u"password"});
    co_return result_;
  }

 private:
  StatusOr<AuthenticationResult> result_;
};

TEST(AuthenticationAdaptersTest, MakeAuthenticatorResolvesAsyncSuccess) {
  auto executor = std::make_shared<TestExecutor>(true);

  auto authenticator =
      MakeAuthenticator(MakeTestAnyExecutor(executor),
                        [](LocalizedText, LocalizedText)
                            -> Awaitable<StatusOr<AuthenticationResult>> {
                          co_return AuthenticationResult{
                              .user_id = NodeId{1, 2},
                              .user_rights = 7,
                              .multi_sessions = true};
                        });

  auto result = WaitPromise(executor, authenticator(u"user", u"password"));

  EXPECT_EQ(result.user_id, (NodeId{1, 2}));
  EXPECT_EQ(result.user_rights, 7u);
  EXPECT_TRUE(result.multi_sessions);
}

TEST(AuthenticationAdaptersTest, MakeAuthenticatorRejectsAsyncBadStatus) {
  auto executor = std::make_shared<TestExecutor>(true);

  auto authenticator =
      MakeAuthenticator(MakeTestAnyExecutor(executor),
                        [](LocalizedText, LocalizedText)
                            -> Awaitable<StatusOr<AuthenticationResult>> {
                          co_return StatusCode::Bad_WrongLoginCredentials;
                        });

  try {
    (void)WaitPromise(executor, authenticator(u"user", u"password"));
    ADD_FAILURE() << "Expected rejected status promise";
  } catch (const status_exception& e) {
    EXPECT_EQ(e.status().code(),
              StatusCode::Bad_WrongLoginCredentials);
  }
}

TEST(AuthenticationAdaptersTest, MakeAsyncAuthenticatorAwaitsLegacySuccess) {
  auto executor = std::make_shared<TestExecutor>(true);

  auto authenticator =
      MakeAsyncAuthenticator(MakeTestAnyExecutor(executor),
                             [](const LocalizedText&, const LocalizedText&) {
                               return make_resolved_promise(
                                   AuthenticationResult{
                                       .user_id = NodeId{3, 4},
                                       .user_rights = 9,
                                       .multi_sessions = false});
                             });

  auto result = WaitAwaitable(executor, authenticator(u"user", u"password"));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(result->user_id, (NodeId{3, 4}));
  EXPECT_EQ(result->user_rights, 9u);
  EXPECT_FALSE(result->multi_sessions);
}

TEST(AuthenticationAdaptersTest,
     MakeAsyncAuthenticatorReturnsRejectedLegacyStatus) {
  auto executor = std::make_shared<TestExecutor>(true);

  auto authenticator =
      MakeAsyncAuthenticator(MakeTestAnyExecutor(executor),
                             [](const LocalizedText&, const LocalizedText&) {
                               return MakeRejectedStatusPromise<
                                   AuthenticationResult>(
                                   StatusCode::Bad_WrongLoginCredentials);
                             });

  auto result = WaitAwaitable(executor, authenticator(u"user", u"password"));

  ASSERT_FALSE(result.ok());
  EXPECT_EQ(result.status().code(),
            StatusCode::Bad_WrongLoginCredentials);
}

TEST(AuthenticationAdaptersTest,
     MakeAuthenticatorAdaptsCoroutineAuthenticatorInterface) {
  auto executor = std::make_shared<TestExecutor>(true);
  StubCoroutineAuthenticator authenticator{AuthenticationResult{
      .user_id = NodeId{5, 6}, .user_rights = 11, .multi_sessions = true}};

  auto legacy_authenticator =
      MakeAuthenticator(MakeTestAnyExecutor(executor), authenticator);

  auto result =
      WaitPromise(executor, legacy_authenticator(u"user", u"password"));

  EXPECT_EQ(result.user_id, (NodeId{5, 6}));
  EXPECT_EQ(result.user_rights, 11u);
  EXPECT_TRUE(result.multi_sessions);
}

TEST(AuthenticationAdaptersTest,
     MakeCoroutineAuthenticatorWrapsLegacyAuthenticator) {
  auto executor = std::make_shared<TestExecutor>(true);
  auto coroutine_authenticator = MakeCoroutineAuthenticator(
      MakeTestAnyExecutor(executor),
      [](const LocalizedText&, const LocalizedText&) {
        return make_resolved_promise(AuthenticationResult{
            .user_id = NodeId{7, 8}, .user_rights = 13, .multi_sessions = false});
      });

  auto result = WaitAwaitable(executor,
                              coroutine_authenticator->Authenticate(u"user",
                                                                    u"password"));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(result->user_id, (NodeId{7, 8}));
  EXPECT_EQ(result->user_rights, 13u);
  EXPECT_FALSE(result->multi_sessions);
}

}  // namespace
}  // namespace scada
