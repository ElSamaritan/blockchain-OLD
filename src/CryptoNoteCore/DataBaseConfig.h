// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Develops
//
// Please see the included LICENSE file for more information.

#pragma once

#include <cstdint>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <string>

#include <Xi/Config/NetworkType.h>
#include <Common/StringTools.h>
#include <Serialization/ISerializer.h>
#include <Serialization/EnumSerialization.hpp>

namespace CryptoNote {

class DataBaseConfig {
 public:
  enum struct Compression { None = 1, LZ4, LZ4HC };

 public:
  static bool parseCompression(const std::string& compressionMode, Compression& out);

 public:
  DataBaseConfig();
  bool init(const std::string dataDirectory, const uint16_t backgroundThreads, const uint16_t maxOpenFiles,
            const uint64_t writeBufferSize, const uint64_t readCacheSize);

  bool isConfigFolderDefaulted() const;
  std::string getDataDir() const;
  uint16_t getBackgroundThreadsCount() const;
  uint32_t getMaxOpenFiles() const;
  uint64_t getWriteBufferSize() const;  // Bytes
  uint64_t getReadCacheSize() const;    // Bytes
  Compression getCompression() const;

  void setConfigFolderDefaulted(bool defaulted);
  void setDataDir(const std::string& dataDir);
  void setBackgroundThreadsCount(uint16_t backgroundThreadsCount);
  void setMaxOpenFiles(uint32_t maxOpenFiles);
  void setWriteBufferSize(uint64_t writeBufferSize);  // Bytes
  void setReadCacheSize(uint64_t readCacheSize);      // Bytes
  void setCompression(Compression compression);

 private:
  bool configFolderDefaulted;
  std::string dataDir;
  uint16_t backgroundThreadsCount;
  uint32_t maxOpenFiles;
  uint64_t writeBufferSize;
  uint64_t readCacheSize;
  Compression compression;
};

XI_SERIALIZATION_ENUM(DataBaseConfig::Compression)
[[nodiscard]] bool serialize(DataBaseConfig::Compression& compression, ISerializer& s);

std::string toString(DataBaseConfig::Compression com);
}  // namespace CryptoNote

namespace Common {
template <>
void toString<CryptoNote::DataBaseConfig::Compression>(const CryptoNote::DataBaseConfig::Compression& compression,
                                                       std::string& out);
}

XI_SERIALIZATION_ENUM_RANGE(CryptoNote::DataBaseConfig::Compression, None, LZ4HC)
XI_SERIALIZATION_ENUM_TAG(CryptoNote::DataBaseConfig::Compression, None, "none")
XI_SERIALIZATION_ENUM_TAG(CryptoNote::DataBaseConfig::Compression, LZ4, "lz4")
XI_SERIALIZATION_ENUM_TAG(CryptoNote::DataBaseConfig::Compression, LZ4HC, "lz4hc")
