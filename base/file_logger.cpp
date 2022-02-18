#include "base/file_logger.h"

#include "base/file_path_util.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"

#include <algorithm>
#include <vector>

FileLogger::FileLogger(std::filesystem::path path, std::u16string base_name)
    : max_files_(10),
      max_file_size_(200 * 1024),
      path_{std::move(path)},
      base_name_{std::move(base_name)} {
  CreateNewFile();
}

FileLogger::~FileLogger() {
  CloseHandle();
}

void FileLogger::Write(LogSeverity severity, const char* message) const {
  assert(!base_name_.empty());
  if (!file_.IsValid())
    return;

  base::Time::Exploded time = {0};
  base::Time::Now().LocalExplode(&time);

  static const char kSeverityNames[] = "NWEF";
  static_assert(
      arraysize(kSeverityNames) == static_cast<size_t>(LogSeverity::Count) + 1,
      "Not all severities are described");

  std::string line = base::StringPrintf(
      "%02d:%02d:%02d.%03d | %c | %s\r\n", time.hour, time.minute, time.second,
      time.millisecond, kSeverityNames[static_cast<int>(severity)], message);

  _Write(line.data(), line.length());

  last_time_ = time;
}

void FileLogger::WriteF(LogSeverity severity, const char* format, ...) const {
  va_list args;
  va_start(args, format);
  WriteV(severity, format, args);
  va_end(args);
}

void FileLogger::WriteV(LogSeverity severity,
                        const char* format,
                        va_list args) const {
  if (!file_.IsValid())
    return;

  std::string line = base::StringPrintV(format, args);
  Write(severity, line.c_str());
}

std::filesystem::path FileLogger::GenerateFileName() const {
  base::Time::Exploded time = {0};
  base::Time::Now().LocalExplode(&time);
  return base_name_ + base::StringPrintf(u"_%d%02d%02d_%02d%02d%02d.log",
                                         time.year, time.month,
                                         time.day_of_month, time.hour,
                                         time.minute, time.second);
}

void FileLogger::CreateNewFile() const {
  CloseHandle();

  DeleteOldFiles();

  base::CreateDirectory(AsFilePath(path_));

  std::filesystem::path path = path_ / GenerateFileName();

  file_.Initialize(AsFilePath(path), base::File::FLAG_CREATE_ALWAYS |
                                         base::File::FLAG_WRITE |
                                         base::File::FLAG_EXCLUSIVE_WRITE);
}

void FileLogger::_Write(const char* data, size_t size) const {
  assert(size > 0);
  if (!file_.IsValid())
    return;

  file_.Write(-1, data, size);

  base::File::Info file_info;
  if (!file_.GetInfo(&file_info))
    return;

  int64 file_size = file_info.size;
  if (file_size >= max_file_size_)
    CreateNewFile();
}

void FileLogger::DeleteOldFiles() const {
  if (max_files_ == 0)
    return;

  std::vector<base::FilePath> paths;
  paths.reserve(16);

  base::FileEnumerator file_enumerator(AsFilePath(path_), false,
                                       base::FileEnumerator::FILES,
                                       base::AsWString(base_name_ + u"*.log"));
  base::FilePath path;
  while (!(path = file_enumerator.Next()).empty())
    paths.emplace_back(std::move(path));

  if (paths.size() < max_files_)
    return;

  std::sort(paths.begin(), paths.end());

  for (size_t i = 0; i < paths.size() - max_files_; ++i)
    base::DeleteFile(paths[i], false);
}

void FileLogger::CloseHandle() const {
  file_.Close();
}
