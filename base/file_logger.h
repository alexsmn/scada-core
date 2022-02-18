#pragma once

#include "base/files/file.h"
#include "base/logger.h"
#include "base/time/time.h"

#include <filesystem>
#include <string>

class FileLogger : public Logger {
 public:
  FileLogger(std::filesystem::path path, std::u16string base_name);
  virtual ~FileLogger();

  FileLogger(const FileLogger&) = delete;
  FileLogger& operator=(const FileLogger&) = delete;

  // Logger overrides
  virtual void Write(LogSeverity severity, const char* message) const override;
  virtual void WriteV(LogSeverity severity,
                      const char* message,
                      va_list args) const override;
  virtual void WriteF(LogSeverity severity,
                      const char* format,
                      ...) const override;

  void _Write(const char* data, size_t size) const;

 private:
  std::filesystem::path GenerateFileName() const;

  void CreateNewFile() const;
  void DeleteOldFiles() const;

  void CloseHandle() const;

  std::filesystem::path path_;
  std::u16string base_name_;
  unsigned max_files_;
  int64 max_file_size_;

  mutable base::File file_;
  mutable base::Time::Exploded last_time_;
};
