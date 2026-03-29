#include "base/path_service.h"

#include <cassert>
#include <mutex>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#pragma comment(lib, "shell32.lib")
#else
#include <unistd.h>
#include <climits>
#endif

namespace base {

namespace {

struct ProviderEntry {
  PathProviderFunc func;
  int key_start;
  int key_end;
};

std::mutex& GetMutex() {
  static std::mutex mutex;
  return mutex;
}

std::vector<ProviderEntry>& GetProviders() {
  static std::vector<ProviderEntry> providers;
  return providers;
}

std::unordered_map<int, std::filesystem::path>& GetOverrides() {
  static std::unordered_map<int, std::filesystem::path> overrides;
  return overrides;
}

}  // namespace

bool PathService::GetBuiltin(int key, std::filesystem::path* result) {
  switch (key) {
    case DIR_CURRENT:
      *result = std::filesystem::current_path();
      return true;

    case DIR_TEMP:
      *result = std::filesystem::temp_directory_path();
      return true;

    case FILE_EXE: {
#ifdef _WIN32
      wchar_t path[MAX_PATH];
      DWORD len = GetModuleFileNameW(nullptr, path, MAX_PATH);
      if (len == 0 || len >= MAX_PATH)
        return false;
      *result = std::filesystem::path{path};
#else
      char path[PATH_MAX];
      ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
      if (len <= 0)
        return false;
      path[len] = '\0';
      *result = std::filesystem::path{path};
#endif
      return true;
    }

    case DIR_EXE: {
      std::filesystem::path exe_path;
      if (!GetBuiltin(FILE_EXE, &exe_path))
        return false;
      *result = exe_path.parent_path();
      return true;
    }

#ifdef _WIN32
    case DIR_APP_DATA: {
      wchar_t path[MAX_PATH];
      if (FAILED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, path)))
        return false;
      *result = std::filesystem::path{path};
      return true;
    }

    case DIR_LOCAL_APP_DATA: {
      wchar_t path[MAX_PATH];
      if (FAILED(
              SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path)))
        return false;
      *result = std::filesystem::path{path};
      return true;
    }

    case DIR_COMMON_APP_DATA: {
      wchar_t path[MAX_PATH];
      if (FAILED(SHGetFolderPathW(nullptr, CSIDL_COMMON_APPDATA, nullptr, 0,
                                   path)))
        return false;
      *result = std::filesystem::path{path};
      return true;
    }

    case DIR_WINDOWS: {
      wchar_t path[MAX_PATH];
      UINT len = GetWindowsDirectoryW(path, MAX_PATH);
      if (len == 0 || len >= MAX_PATH)
        return false;
      *result = std::filesystem::path{path};
      return true;
    }
#endif

    default:
      return false;
  }
}

bool PathService::Get(int key, std::filesystem::path* result) {
  std::lock_guard lock{GetMutex()};

  // Check overrides first.
  auto& overrides = GetOverrides();
  auto it = overrides.find(key);
  if (it != overrides.end()) {
    *result = it->second;
    return true;
  }

  // Try registered providers (in reverse order so later registrations win).
  auto& providers = GetProviders();
  for (auto i = providers.rbegin(); i != providers.rend(); ++i) {
    if (key >= i->key_start && key < i->key_end) {
      if (i->func(key, result))
        return true;
    }
  }

  // Fall back to builtins.
  return GetBuiltin(key, result);
}

void PathService::RegisterProvider(PathProviderFunc provider,
                                   int key_start,
                                   int key_end) {
  std::lock_guard lock{GetMutex()};
  GetProviders().push_back({provider, key_start, key_end});
}

void PathService::Override(int key, const std::filesystem::path& path) {
  std::lock_guard lock{GetMutex()};
  GetOverrides()[key] = path;
}

}  // namespace base
