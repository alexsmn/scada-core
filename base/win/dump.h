#pragma once

#include "base/base_export.h"

#include <string>
#include <windows.h>

BASE_EXPORT bool DumpException(const wchar_t* path,
                               const EXCEPTION_POINTERS& exc);

BASE_EXPORT int DumpExceptionFilter(DWORD code, _EXCEPTION_POINTERS* exc);
