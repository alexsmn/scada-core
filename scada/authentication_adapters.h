#pragma once

#include "base/any_executor.h"
#include "base/awaitable_promise.h"
#include "scada/authentication.h"
#include "scada/status_exception.h"

#include <memory>

namespace scada {

inline AsyncAuthenticator MakeAsyncAuthenticator(AnyExecutor executor,
                                                 Authenticator authenticator);

class FunctionCoroutineAuthenticator final : public CoroutineAuthenticator {
 public:
  explicit FunctionCoroutineAuthenticator(AsyncAuthenticator authenticator)
      : authenticator_{std::move(authenticator)} {}

  Awaitable<StatusOr<AuthenticationResult>> Authenticate(
      LocalizedText user_name,
      LocalizedText password) override {
    co_return co_await authenticator_(std::move(user_name),
                                      std::move(password));
  }

 private:
  AsyncAuthenticator authenticator_;
};

inline std::shared_ptr<CoroutineAuthenticator> MakeCoroutineAuthenticator(
    AsyncAuthenticator authenticator) {
  return std::make_shared<FunctionCoroutineAuthenticator>(
      std::move(authenticator));
}

inline std::shared_ptr<CoroutineAuthenticator> MakeCoroutineAuthenticator(
    AnyExecutor executor,
    Authenticator authenticator) {
  return MakeCoroutineAuthenticator(
      MakeAsyncAuthenticator(std::move(executor), std::move(authenticator)));
}

inline AsyncAuthenticator MakeAsyncAuthenticator(
    CoroutineAuthenticator& authenticator) {
  return [&authenticator](LocalizedText user_name,
                          LocalizedText password)
             -> Awaitable<StatusOr<AuthenticationResult>> {
    co_return co_await authenticator.Authenticate(std::move(user_name),
                                                  std::move(password));
  };
}

inline Authenticator MakeAuthenticator(AnyExecutor executor,
                                       AsyncAuthenticator async_authenticator) {
  return [executor = std::move(executor),
          async_authenticator = std::move(async_authenticator)](
             const LocalizedText& user_name,
             const LocalizedText& password) mutable {
    return ToPromise(
        executor,
        [async_authenticator = async_authenticator, user_name, password]()
            mutable -> Awaitable<AuthenticationResult> {
          auto result =
              co_await async_authenticator(std::move(user_name),
                                           std::move(password));
          if (!result.ok()) {
            throw status_exception{result.status()};
          }
          co_return std::move(*result);
        }());
  };
}

inline Authenticator MakeAuthenticator(AnyExecutor executor,
                                       CoroutineAuthenticator& authenticator) {
  return MakeAuthenticator(std::move(executor),
                           MakeAsyncAuthenticator(authenticator));
}

inline AsyncAuthenticator MakeAsyncAuthenticator(AnyExecutor executor,
                                                 Authenticator authenticator) {
  return [executor = std::move(executor),
          authenticator = std::move(authenticator)](
             LocalizedText user_name,
             LocalizedText password)
             -> Awaitable<StatusOr<AuthenticationResult>> {
    try {
      co_return co_await AwaitPromise(executor,
                                      authenticator(std::move(user_name),
                                                    std::move(password)));
    } catch (...) {
      co_return GetExceptionStatus(std::current_exception());
    }
  };
}

}  // namespace scada
