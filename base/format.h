#pragma once

#include "base/strings/string_piece.h"
#include "base/strings/stringprintf.h"
#include "build/build_config.h"

#include <cassert>
#include <exception>
#include <stdarg.h>
#include <string>

std::string Format(double value);
std::wstring WideFormat(double value);

std::string Format(float value);
std::wstring WideFormat(float value);

std::string Format(int32_t value);
std::wstring WideFormat(int32_t value);

std::string Format(uint32_t value);
std::wstring WideFormat(uint32_t value);

std::string Format(int64_t value);
std::wstring WideFormat(int64_t value);

std::string Format(uint64_t value);
std::wstring WideFormat(uint64_t value);

std::string Format(bool value);
std::wstring WideFormat(bool value);

std::string Format(uint8_t value);
std::wstring WideFormat(uint8_t value);

std::string Format(uint16_t value);
std::wstring WideFormat(uint16_t value);

std::string Format(base::StringPiece value);
std::wstring WideFormat(base::StringPiece value);

std::string Format(base::StringPiece16 value);
std::wstring WideFormat(base::StringPiece16 value);

template <typename T>
bool Parse(const base::StringPiece& str, T& value);
template <typename T>
bool Parse(const base::StringPiece16& str, T& value);

template <typename T>
inline T ParseWithDefault(const base::StringPiece& str, T def) {
  T value;
  return Parse(str, value) ? value : def;
}

template <typename T>
inline T ParseWithDefault(const base::StringPiece16& str, T def) {
  T value;
  return Parse(str, value) ? value : def;
}

template <typename T>
inline T ParseT(const base::StringPiece& str) {
  T value;
  if (!Parse<T>(str, value))
    throw std::exception();
  return value;
}

template <typename T>
inline T ParseT(const base::StringPiece16& str) {
  T value;
  if (!Parse<T>(str, value))
    throw std::exception();
  return value;
}

std::string FormatHexBuffer(const void* buf, size_t len);
