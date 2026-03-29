#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace base {

class Pickle {
 public:
  Pickle() = default;

  Pickle(const char* data, int data_len)
      : data_(data, data + data_len) {}

  const char* data() const { return data_.data(); }
  size_t size() const { return data_.size(); }

  void WriteUInt16(uint16_t value) { WriteBytes(&value, sizeof(value)); }
  void WriteUInt32(uint32_t value) { WriteBytes(&value, sizeof(value)); }
  void WriteString(const std::string& value) {
    WriteUInt32(static_cast<uint32_t>(value.size()));
    WriteBytes(value.data(), value.size());
  }
  void WriteString16(const std::u16string& value) {
    WriteUInt32(static_cast<uint32_t>(value.size()));
    WriteBytes(value.data(), value.size() * sizeof(char16_t));
  }

 private:
  void WriteBytes(const void* data, size_t len) {
    auto* bytes = static_cast<const char*>(data);
    data_.insert(data_.end(), bytes, bytes + len);
  }

  std::vector<char> data_;
};

class PickleIterator {
 public:
  explicit PickleIterator(const Pickle& pickle)
      : data_(pickle.data()), end_(pickle.data() + pickle.size()) {}

  bool ReadUInt16(uint16_t* result) { return ReadPod(result); }
  bool ReadUInt32(uint32_t* result) { return ReadPod(result); }
  bool ReadString(std::string* result) {
    uint32_t len;
    if (!ReadPod(&len))
      return false;
    if (data_ + len > end_)
      return false;
    result->assign(data_, len);
    data_ += len;
    return true;
  }
  bool ReadString16(std::u16string* result) {
    uint32_t len;
    if (!ReadPod(&len))
      return false;
    size_t byte_len = len * sizeof(char16_t);
    if (data_ + byte_len > end_)
      return false;
    result->resize(len);
    std::memcpy(result->data(), data_, byte_len);
    data_ += byte_len;
    return true;
  }

 private:
  template <typename T>
  bool ReadPod(T* result) {
    if (data_ + sizeof(T) > end_)
      return false;
    std::memcpy(result, data_, sizeof(T));
    data_ += sizeof(T);
    return true;
  }

  const char* data_;
  const char* end_;
};

}  // namespace base
