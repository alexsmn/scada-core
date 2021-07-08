#pragma once

#include <string>
#include <windows.h>

std::string FormatHresult(HRESULT err);
std::wstring FormatHresultUTF16(HRESULT err);
