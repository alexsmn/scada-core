#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/callback_awaitable.h"
#include "scada/status.h"

#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace scada {

template <class T, class Start>
[[nodiscard]] Awaitable<T> AwaitCallbackValue(AnyExecutor executor,
                                              Start&& start) {
  auto [value] = co_await CallbackToAwaitable<T>(
      std::move(executor), std::forward<Start>(start));
  co_return std::move(value);
}

template <class... Args, class Start>
[[nodiscard]] Awaitable<std::tuple<std::decay_t<Args>...>> AwaitCallbackTuple(
    AnyExecutor executor,
    Start&& start) {
  co_return co_await CallbackToAwaitable<Args...>(
      std::move(executor), std::forward<Start>(start));
}

template <class Start>
[[nodiscard]] Awaitable<Status> AwaitStatusCallback(AnyExecutor executor,
                                                    Start&& start) {
  co_return co_await AwaitCallbackValue<Status>(std::move(executor),
                                                std::forward<Start>(start));
}

template <class Start>
[[nodiscard]] Awaitable<std::tuple<Status, std::vector<StatusCode>>>
AwaitStatusCodesCallback(AnyExecutor executor, Start&& start) {
  co_return co_await AwaitCallbackTuple<Status, std::vector<StatusCode>>(
      std::move(executor), std::forward<Start>(start));
}

}  // namespace scada
