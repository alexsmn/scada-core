#pragma once

#include <base/files/file_path.h>
#include <filesystem>

inline base::FilePath AsFilePath(const std::filesystem::path& path) {
#if defined(WIN32)
  return base::FilePath{path.wstring()};
#else
  return base::FilePath{path.string()};
#endif
}

inline std::filesystem::path AsFilesystemPath(const base::FilePath& path) {
  return path.value();
}
