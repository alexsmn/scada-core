#pragma once

#include "base/containers/span.h"

#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

template <class A, class B>
std::ostream& operator<<(std::ostream& stream, const std::pair<A, B>& pair);

template <class T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v);

template <class T>
std::ostream& operator<<(std::ostream& stream, base::span<T> span);

template <class T>
std::ostream& operator<<(std::ostream& stream, const std::optional<T>& v);

template <class T>
std::string ToString(const T& v);

template <class T>
std::wstring ToString16(const T& v);

std::string BitMaskToString(unsigned bit_mask,
                            base::span<const std::string_view> bit_strings);
