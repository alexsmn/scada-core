#include "scada/authentication_adapters.h"

#include "base/test/awaitable_test.h"
#include "scada/status_promise.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

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

}  // namespace
}  // namespace scada
