#pragma once

#include <boost/json.hpp>
#include <filesystem>
#include <fstream>

inline std::optional<boost::json::value> ReadBoostJsonFromFile(
    const std::filesystem::path& path) {
  // TODO: Avoid streams.
  std::ifstream stream{path};
  if (!stream) {
    return std::nullopt;
  }
  std::error_code ec;
  auto value = boost::json::parse(stream, ec);
  return ec ? std::nullopt : std::make_optional(std::move(value));
}

inline void WriteBoostJsonToFile(const boost::json::value& json,
                                 const std::filesystem::path& path) {
  // TODO: Avoid streams.
  std::ofstream stream{path, std::ios::binary | std::ios::out};
  stream << boost::json::serialize(json);
}