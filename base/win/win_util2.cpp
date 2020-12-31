#include "base/win/win_util2.h"

#include "base/format.h"
#include "base/logging.h"
#include "base/strings/string_util.h"

#include <memory>
#include <shellapi.h>

namespace win_util {

void InsertMenuItem(HMENU menu, int pos, UINT id, LPCTSTR text, UINT state) {
  MENUITEMINFO mii;
  memset(&mii, 0, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_STATE;
  mii.fType = MFT_STRING;
  mii.fState = state;
  mii.wID = id;
  mii.dwTypeData = (LPTSTR)text;
  InsertMenuItem(menu, pos, TRUE, &mii);
}

void InsertSubMenu(HMENU menu, int pos, const wchar_t* text, HMENU submenu) {
  MENUITEMINFO mii;
  memset(&mii, 0, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU;
  mii.fType = MFT_STRING;
  mii.dwTypeData = (LPTSTR)text;
  mii.hSubMenu = submenu;
  InsertMenuItem(menu, pos, TRUE, &mii);
}

void InsertMenuSeparator(HMENU menu, int pos) {
  MENUITEMINFO mii;
  memset(&mii, 0, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_FTYPE;
  mii.fType = MFT_SEPARATOR;
  InsertMenuItem(menu, pos, TRUE, &mii);
}

void RemoveConsequentMenuSeparators(HMENU menu) {
  if (GetMenuItemCount(menu) == 0)
    return;

  MENUITEMINFO mii;
  memset(&mii, 0, sizeof(mii));
  mii.cbSize = sizeof(mii);
  mii.fMask = MIIM_FTYPE;

  int count;
  while ((count = GetMenuItemCount(menu)) > 0 &&
         GetMenuItemInfo(menu, 0, TRUE, &mii) && mii.fType == MFT_SEPARATOR) {
    DeleteMenu(menu, 0, MF_BYPOSITION);
  }

  while ((count = GetMenuItemCount(menu)) > 0 &&
         GetMenuItemInfo(menu, count - 1, TRUE, &mii) &&
         mii.fType == MFT_SEPARATOR) {
    DeleteMenu(menu, count - 1, MF_BYPOSITION);
  }

  // Remove consequent separators.
  bool last_sep = false;
  for (int i = 0; i < GetMenuItemCount(menu);) {
    GetMenuItemInfo(menu, i, TRUE, &mii);
    if (mii.fType == MFT_SEPARATOR) {
      if (last_sep) {
        DeleteMenu(menu, i, MF_BYPOSITION);
        last_sep = false;
        continue;
      }
      last_sep = true;
    } else
      last_sep = false;
    i++;
  }
}

int RemoveMenuCommand(HMENU menu, UINT command) {
  int pos = -1;
  for (int i = 0; i < GetMenuItemCount(menu); i++)
    if (GetMenuItemID(menu, i) == command) {
      pos = i;
      break;
    }

  if (pos != -1)
    DeleteMenu(menu, pos, MF_BYPOSITION);
  return pos;
}

std::wstring LoadResourceString(HMODULE module, unsigned id) {
  wchar_t* buffer = NULL;
  int len = ::LoadStringW(module, id, reinterpret_cast<LPWSTR>(&buffer), 0);
  return std::wstring(buffer, len);
}

std::wstring GetWindowText(HWND window_handle) {
  DCHECK(IsWindow(window_handle));
  int len = GetWindowTextLength(window_handle) + 1;
  std::wstring str;
  if (len > 1)
    ::GetWindowText(window_handle, base::WriteInto(&str, len), len);
  return str;
}

int GetWindowInt(HWND window_handle) {
  std::wstring str = GetWindowText(window_handle);
  int value;
  if (!Parse(str, value))
    throw E_INVALIDARG;
  return value;
}

void SetWindowTextInt(HWND window_handle, int value) {
  SetWindowText(window_handle, WideFormat(value).c_str());
}

std::wstring GetListBoxItemText(HWND window_handle, int index) {
  int len = SendMessage(window_handle, LB_GETTEXTLEN, index, 0L);
  if (len <= 0)
    return std::wstring();

  std::unique_ptr<wchar_t[]> text(new wchar_t[len + 1]);
  int res = SendMessage(window_handle, LB_GETTEXT, index,
                        reinterpret_cast<LPARAM>(text.get()));
  if (res == LB_ERR)
    return std::wstring();
  return std::wstring(text.get(), text.get() + len);
}

std::wstring GetComboBoxItemText(HWND window_handle, int index) {
  int len = SendMessage(window_handle, CB_GETLBTEXTLEN, index, 0L);
  if (len <= 0)
    return std::wstring();

  std::unique_ptr<wchar_t[]> text(new wchar_t[len + 1]);
  int res = SendMessage(window_handle, CB_GETLBTEXT, index,
                        reinterpret_cast<LPARAM>(text.get()));
  if (res == LB_ERR)
    return std::wstring();
  return std::wstring(text.get(), text.get() + len);
}

void OpenWithAssociatedProgram(const std::filesystem::path& path) {
  ShellExecute(NULL, NULL, path.c_str(), NULL, NULL, SW_SHOW);
}

}  // namespace win_util
