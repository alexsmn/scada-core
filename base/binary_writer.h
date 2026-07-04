#pragma once

#include <bit>
#include <cstdint>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string_view>

// Sequential writer into a fixed-capacity, caller-owned byte buffer.
// Successor of the write half of the legacy ByteMessage/ByteBuffer. All
// writes advance the written size and throw std::runtime_error when they
// would exceed the buffer capacity. The writer does not own the memory.
class BinaryWriter {
 public:
  explicit BinaryWriter(std::span<uint8_t> buffer) : buffer_{buffer} {}
  explicit BinaryWriter(std::span<char> buffer)
      : buffer_{reinterpret_cast<uint8_t*>(buffer.data()), buffer.size()} {}

  // Number of bytes written so far.
  size_t size() const { return size_; }

  // Remaining capacity.
  size_t capacity_left() const { return buffer_.size() - size_; }

  // View of the bytes written so far.
  std::span<const uint8_t> written() const { return buffer_.first(size_); }

  // Reserves the next `size` bytes for the caller to fill and advances the
  // written size. Throws on capacity overflow.
  std::span<uint8_t> AllocateBytes(size_t size) {
    Require(size);
    auto span = buffer_.subspan(size_, size);
    size_ += size;
    return span;
  }

  // Appends `bytes` to the buffer.
  void WriteBytes(std::span<const uint8_t> bytes) {
    auto span = AllocateBytes(bytes.size());
    if (!bytes.empty())
      memcpy(span.data(), bytes.data(), bytes.size());
  }

  // Fixed-width writes in native byte order.
  void WriteU8(uint8_t value) { WriteT(value); }
  void WriteU16(uint16_t value) { WriteT(value); }
  void WriteU32(uint32_t value) { WriteT(value); }

  // Fixed-width writes of big-endian (network order) values.
  void WriteU16Be(uint16_t value) { WriteU16(ToBigEndian(value)); }
  void WriteU32Be(uint32_t value) { WriteU32(ToBigEndian(value)); }

  // Writes a uint8-length-prefixed string (readable back via
  // BinaryReader::ReadString8). Throws if `str` is longer than 255 bytes.
  void WriteString8(std::string_view str) {
    if (str.size() > 255)
      throw std::runtime_error("Too long message string");
    WriteU8(static_cast<uint8_t>(str.size()));
    WriteBytes({reinterpret_cast<const uint8_t*>(str.data()), str.size()});
  }

 private:
  // Throws unless `size` more bytes fit into the buffer.
  void Require(size_t size) const {
    if (size > capacity_left())
      throw std::runtime_error("Write error");
  }

  template <typename T>
    requires std::is_trivially_copyable_v<T>
  void WriteT(const T& value) {
    memcpy(AllocateBytes(sizeof(value)).data(), &value, sizeof(value));
  }

  template <typename T>
  static T ToBigEndian(T value) {
    if constexpr (std::endian::native == std::endian::little)
      return std::byteswap(value);
    else
      return value;
  }

  std::span<uint8_t> buffer_;
  size_t size_ = 0;
};
