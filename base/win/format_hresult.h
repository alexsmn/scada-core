#pragma once

#include "base/base_export.h"

#include <string>
#include <windows.h>

std::string BASE_EXPORT FormatHresult(HRESULT err);
std::wstring BASE_EXPORT FormatHresultUTF16(HRESULT err);
