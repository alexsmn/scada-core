#pragma once

#include <string_view>
#include <windows.h>

std::string GetDumpFileName(std::string_view prefix);

bool DumpException(const wchar_t* path, const EXCEPTION_POINTERS& exc);

int DumpExceptionFilter(DWORD code, _EXCEPTION_POINTERS* exc);
