#pragma once

#include "core/variant.h"

#include <span>

namespace scada {

namespace internal {

inline bool UnpackMethodArguments(std::span<const Variant> arguments,
                                  std::tuple<>& result) {
  return arguments.empty();
}

template <class A>
inline bool UnpackMethodArguments(std::span<const Variant> arguments,
                                  std::tuple<A>& result) {
  if (arguments.size() != 1)
    return false;

  if (!arguments[0].get(std::get<0>(result)))
    return false;

  return true;
}

template <class A, class B>
inline bool UnpackMethodArguments(std::span<const Variant> arguments,
                                  std::tuple<A, B>& result) {
  if (arguments.size() != 2)
    return false;

  if (!arguments[0].get(std::get<0>(result)))
    return false;

  if (!arguments[1].get(std::get<1>(result)))
    return false;

  return true;
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
