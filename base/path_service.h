#pragma once

#include <filesystem>
#include <functional>

namespace base {

enum {
  DIR_CURRENT,
  DIR_EXE,
  FILE_EXE,
  DIR_TEMP,
#ifdef _WIN32
  DIR_APP_DATA,
  DIR_LOCAL_APP_DATA,
  DIR_COMMON_APP_DATA,
  DIR_WINDOWS,
#endif
  PATH_BASE_END,
};

using PathProviderFunc = bool (*)(int key, std::filesystem::path* result);

class PathService {
 public:
  static bool Get(int key, std::filesystem::path* result);
  static void RegisterProvider(PathProviderFunc provider,
                               int key_start,
                               int key_end);
  static void Override(int key, const std::filesystem::path& path);

 private:
  static bool GetBuiltin(int key, std::filesystem::path* result);
};

}  // namespace base
