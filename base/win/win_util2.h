#pragma once

#include <filesystem>
#include <string>
#include <windows.h>

namespace win_util {

void InsertMenuItem(HMENU menu,
                    int pos,
                    UINT id,
                    const wchar_t* text,
                    UINT state = 0);
void InsertSubMenu(HMENU menu, int pos, const wchar_t* text, HMENU submenu);
void InsertMenuSeparator(HMENU menu, int pos);

void RemoveConsequentMenuSeparators(HMENU menu);

// Find and remove menu item by command.
int RemoveMenuCommand(HMENU menu, UINT command);

std::wstring LoadResourceString(HMODULE module, unsigned id);

std::wstring GetWindowText(HWND window_handle);
int GetWindowInt(HWND window_handle);

void SetWindowTextInt(HWND window_handle, int value);

std::wstring GetListBoxItemText(HWND window_handle, int index);
std::wstring GetComboBoxItemText(HWND window_handle, int index);

void OpenWithAssociatedProgram(const std::filesystem::path& path);

}  // namespace win_util
