#pragma once

#include <cinttypes>

#include <string>

namespace Xi {
namespace Compression {
namespace Gzip {
inline constexpr int minimumCompressionLevel() { return 0; }
inline constexpr int maximumCompressionLevel() { return 9; }
inline constexpr int defaultCompressionLevel() { return -1; }

std::string decompress(const char* data, uint64_t size);
std::string decompress(const std::string& data);
std::string compress(const char* data, uint64_t, int level = defaultCompressionLevel());
std::string compress(const std::string& data, int level = defaultCompressionLevel());
bool isCompressed(const char* data, uint64_t size);
bool isCompressed(const std::string& data);
}  // namespace Gzip
}  // namespace Compression
}  // namespace Xi
