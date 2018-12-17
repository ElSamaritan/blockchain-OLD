#include "Xi/Compression/Gzip.h"

#include "GzipComressor.h"
#include "GzipDecompressor.h"

std::string Xi::Compression::Gzip::decompress(const char* data, uint64_t size) {
  Decompressor decomp;
  std::string output;
  decomp.decompress(output, data, size);
  return output;
}

std::string Xi::Compression::Gzip::decompress(const std::string& data) { return decompress(data.data(), data.size()); }

std::string Xi::Compression::Gzip::compress(const char* data, uint64_t size, int level) {
  if (level < minimumCompressionLevel() && level > maximumCompressionLevel() && level != defaultCompressionLevel())
    throw std::invalid_argument{"compression level is out of range"};
  Compressor comp(level);
  std::string output;
  comp.compress(output, data, size);
  return output;
}

std::string Xi::Compression::Gzip::compress(const std::string& data, int level) {
  return compress(data.data(), data.size(), level);
}

bool Xi::Compression::Gzip::isCompressed(const char* data, uint64_t size) {
  return size > 2 && ((static_cast<uint8_t>(data[0]) == 0x78 &&
                       (static_cast<uint8_t>(data[1]) == 0x9C || static_cast<uint8_t>(data[1]) == 0x01 ||
                        static_cast<uint8_t>(data[1]) == 0xDA || static_cast<uint8_t>(data[1]) == 0x5E)) ||
                      (static_cast<uint8_t>(data[0]) == 0x1F && static_cast<uint8_t>(data[1]) == 0x8B));
}

bool Xi::Compression::Gzip::isCompressed(const std::string& data) { return isCompressed(data.data(), data.size()); }
