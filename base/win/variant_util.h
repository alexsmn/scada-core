#pragma once

#include <Windows.h>

inline VARIANT_BOOL AsVariantBool(bool b) {
  return b ? VARIANT_TRUE : VARIANT_FALSE;
}