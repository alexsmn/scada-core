#pragma once

#include "base/base_export.h"

#include <filesystem>
#include <string>
#include <windows.h>

namespace win_util {

BASE_EXPORT void InsertMenuItem(HMENU menu,
                                int pos,
                                UINT id,
                                const wchar_t* text,
                                UINT state = 0);
BASE_EXPORT void InsertSubMenu(HMENU menu,
                               int pos,
                               const wchar_t* text,
                               HMENU submenu);
BASE_EXPORT void InsertMenuSeparator(HMENU menu, int pos);

BASE_EXPORT void RemoveConsequentMenuSeparators(HMENU menu);

// Find and remove menu item by command.
BASE_EXPORT int RemoveMenuCommand(HMENU menu, UINT command);

BASE_EXPORT std::wstring LoadResourceString(HMODULE module, unsigned id);

BASE_EXPORT std::wstring GetWindowText(HWND window_handle);
BASE_EXPORT int GetWindowInt(HWND window_handle);

BASE_EXPORT void SetWindowTextInt(HWND window_handle, int value);

BASE_EXPORT std::wstring GetListBoxItemText(HWND window_handle, int index);
BASE_EXPORT std::wstring GetComboBoxItemText(HWND window_handle, int index);

BASE_EXPORT void OpenWithAssociatedProgram(const std::filesystem::path& path);

}  // namespace win_util
