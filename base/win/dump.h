#pragma once

#include <windows.h>

bool DumpException(const wchar_t* path, const EXCEPTION_POINTERS& exc);

int DumpExceptionFilter(DWORD code, _EXCEPTION_POINTERS* exc);
