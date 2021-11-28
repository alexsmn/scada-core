#pragma once

#include "core/status.h"

namespace scada {

namespace {

template <class A, class B>
inline StatusCode UnpackMethodArguments(base::span<const Variant> arguments,
                                        std::tuple<A, B>& result) {
  if (arguments.size() != 2)
    return StatusCode::Bad_WrongCallArguments;

  if (!arguments[0].get(std::get<0>(result)))
    return StatusCode::Bad_WrongCallArguments;

  if (!arguments[1].get(std::get<1>(result)))
    return StatusCode::Bad_WrongCallArguments;

  return StatusCode::Good;
}

template <class Instance, class... Args>
inline Status InvokeMethodHelper(base::span<const Variant> arguments,
                                 const Instance& instance,
                                 Status (Instance::*method)(const Args&...)
                                     const) {
  std::tuple<Args...> unpacked_arguments;
  auto unpack_status = UnpackMethodArguments(arguments, unpacked_arguments);
  if (IsBad(unpack_status))
    return unpack_status;

  return (instance.*method)(std::get<0>(unpacked_arguments),
                            std::get<1>(unpacked_arguments));
}

template <class Functor>
inline Status InvokeMethod(base::span<const Variant> arguments,
                           const Functor& functor) {
  return InvokeMethodHelper(arguments, functor, &Functor::operator());
}

}  // namespace

}  // namespace scada
