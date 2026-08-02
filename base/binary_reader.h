#pragma once

#include <bit>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string>

// Sequential reader over a fixed, caller-owned byte buffer. Successor of the
// read half of the legacy ByteMessage. All reads advance the position and
// throw std::runtime_error when they would run past the end of the buffer.
// The reader does not own the memory; the buffer must outlive any views
// returned by RemainingBytes()/ReadBytesView().
class BinaryReader {
 public:
  explicit BinaryReader(std::span<const uint8_t> data) : data_{data} {}
  explicit BinaryReader(std::span<const char> data)
      : data_{reinterpret_cast<const uint8_t*>(data.data()), data.size()} {}

  // Number of unread bytes.
  size_t remaining() const { return data_.size() - pos_; }

  // True when the whole buffer has been consumed.
  bool end() const { return pos_ >= data_.size(); }

  // True when the underlying buffer is empty.
  bool empty() const { return data_.empty(); }

  // Returns a view of the unread remainder without consuming it.
  std::span<const uint8_t> RemainingBytes() const {
    return data_.subspan(pos_);
  }

  // Advances past `size` bytes without reading them.
  void Skip(size_t size) {
    Require(size);
    pos_ += size;
  }

  // Returns a view of the next `size` bytes and advances past them.
  std::span<const uint8_t> ReadBytesView(size_t size) {
    Require(size);
    auto view = data_.subspan(pos_, size);
    pos_ += size;
    return view;
  }

  // Copies `out.size()` bytes into `out` and advances past them.
  void ReadBytes(std::span<uint8_t> out) {
    Require(out.size());
    if (!out.empty())
      memcpy(out.data(), data_.data() + pos_, out.size());
    pos_ += out.size();
  }

  // Fixed-width reads in native byte order.
  uint8_t ReadU8() { return ReadT<uint8_t>(); }
  uint16_t ReadU16() { return ReadT<uint16_t>(); }
  uint32_t ReadU32() { return ReadT<uint32_t>(); }

  // Fixed-width reads of big-endian (network order) values.
  uint16_t ReadU16Be() { return FromBigEndian(ReadU16()); }
  uint32_t ReadU32Be() { return FromBigEndian(ReadU32()); }

  // Reads a uint8-length-prefixed string (the layout produced by
  // BinaryWriter::WriteString8).
  std::string ReadString8() {
    const size_t length = ReadU8();
    auto bytes = ReadBytesView(length);
    return std::string{reinterpret_cast<const char*>(bytes.data()),
                       bytes.size()};
  }

 private:
  // Throws unless `size` more bytes are available.
  void Require(size_t size) const {
    if (size > remaining())
      throw std::runtime_error("read error");
  }

  template <typename T>
    requires std::is_trivially_copyable_v<T>
  T ReadT() {
    T value;
    Require(sizeof(value));
    memcpy(&value, data_.data() + pos_, sizeof(value));
    pos_ += sizeof(value);
    return value;
  }

  template <typename T>
  static T FromBigEndian(T value) {
    if constexpr (std::endian::native == std::endian::little)
      return std::byteswap(value);
    else
      return value;
  }

  std::span<const uint8_t> data_;
  size_t pos_ = 0;
};
