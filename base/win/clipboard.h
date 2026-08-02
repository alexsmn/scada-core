#pragma once

#include <string>

class Clipboard {
 public:
  Clipboard();
  ~Clipboard();

  Clipboard(const Clipboard&) = delete;
  Clipboard& operator=(const Clipboard&) = delete;

  bool Open();
  void Close();

  bool SetData(unsigned format, const void* data, size_t size);
  bool GetData(unsigned format, char*& data, size_t& size);

  bool SetText(const std::string& text);
  bool SetText(const std::wstring& text);

 private:
  bool opened_ = false;
};
