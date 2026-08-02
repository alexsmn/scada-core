#pragma once

#include "scada/variant.h"

#include <cstddef>
#include <span>
#include <tuple>
#include <utility>

namespace scada {

namespace internal {

// Unpacks a Call request's argument list into the tuple a method handler
// expects. Fails when the count differs or any argument does not hold the
// expected type, so a handler never sees a partially converted argument list.
//
// Variadic by design: the standard method signatures range from no arguments
// (RemoveRole) to seven (ModifyUser, OPC UA Part 18 §5.2.6), and a per-arity
// overload set silently stops matching one argument past whatever was written.
template <class... Args>
inline bool UnpackMethodArguments(std::span<const Variant> arguments,
                                  std::tuple<Args...>& result) {
  if (arguments.size() != sizeof...(Args))
    return false;

  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    // Short-circuits, so a later argument is not read once one fails to
    // convert.
    return (arguments[I].get(std::get<I>(result)) && ...);
  }(std::index_sequence_for<Args...>{});
}

template <class Instance, class... Args>
inline bool InvokeMethodHelper(std::span<const Variant> arguments,
                               const Instance& instance,
                               void (Instance::*method)(Args...) const) {
  std::tuple<std::decay_t<Args>...> unpacked_arguments;
  auto ok = UnpackMethodArguments(arguments, unpacked_arguments);
  if (!ok)
    return ok;

  std::apply([&](Args... args) { (instance.*method)(std::move(args)...); },
             unpacked_arguments);

  return true;
}

}  // namespace internal

template <class Functor>
inline bool InvokeMethod(std::span<const Variant> arguments,
                         const Functor& functor) {
  return internal::InvokeMethodHelper(arguments, functor, &Functor::operator());
}

}  // namespace scada
