// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Develops
//
// Please see the included LICENSE file for more information.

#include "DataBaseConfig.h"

#include <limits>

#include <Common/Util.h>
#include "Common/CommandLine.h"
#include "Common/StringTools.h"
#include "crypto/crypto.h"
#include <Xi/Config.h>

using namespace CryptoNote;

bool DataBaseConfig::parseCompression(const std::string &compressionMode, DataBaseConfig::Compression &out) {
  if (compressionMode == "none")
    out = Compression::None;
  else if (compressionMode == "lz4")
    out = Compression::LZ4;
  else if (compressionMode == "lz4hc")
    out = Compression::LZ4HC;
  else
    return false;
  return true;
}

DataBaseConfig::DataBaseConfig()
    : dataDir(Tools::getDefaultDataDirectory()),
      backgroundThreadsCount(Xi::Config::Database::backgroundThreads()),
      maxOpenFiles(Xi::Config::Database::maximumOpenFiles()),
      writeBufferSize(Xi::Config::Database::writeBufferSize()),
      readCacheSize(Xi::Config::Database::readBufferSize()),
      m_network(Xi::Config::Network::defaultNetworkType()),
      compression(Compression::LZ4) {}

bool DataBaseConfig::init(const std::string dataDirectory, const uint16_t backgroundThreads, const uint16_t openFiles,
                          const uint64_t writeBuffer, const uint64_t readCache) {
  dataDir = dataDirectory;

  backgroundThreadsCount = backgroundThreads;
  maxOpenFiles = openFiles;
  writeBufferSize = writeBuffer;
  readCacheSize = readCache;

  if (dataDir == Tools::getDefaultDataDirectory()) {
    configFolderDefaulted = true;
  }
  return true;
}

bool DataBaseConfig::isConfigFolderDefaulted() const { return configFolderDefaulted; }

std::string DataBaseConfig::getDataDir() const { return dataDir; }

uint16_t DataBaseConfig::getBackgroundThreadsCount() const { return backgroundThreadsCount; }

uint32_t DataBaseConfig::getMaxOpenFiles() const { return maxOpenFiles; }

uint64_t DataBaseConfig::getWriteBufferSize() const { return writeBufferSize; }

uint64_t DataBaseConfig::getReadCacheSize() const { return readCacheSize; }

Xi::Config::Network::Type DataBaseConfig::getNetwork() const { return m_network; }

DataBaseConfig::Compression DataBaseConfig::getCompression() const { return compression; }

void DataBaseConfig::setConfigFolderDefaulted(bool defaulted) { configFolderDefaulted = defaulted; }

void DataBaseConfig::setDataDir(const std::string &_dataDir) { this->dataDir = _dataDir; }

void DataBaseConfig::setBackgroundThreadsCount(uint16_t _backgroundThreadsCount) {
  this->backgroundThreadsCount = _backgroundThreadsCount;
}

void DataBaseConfig::setMaxOpenFiles(uint32_t _maxOpenFiles) { this->maxOpenFiles = _maxOpenFiles; }

void DataBaseConfig::setWriteBufferSize(uint64_t _writeBufferSize) { this->writeBufferSize = _writeBufferSize; }

void DataBaseConfig::setReadCacheSize(uint64_t _readCacheSize) { this->readCacheSize = _readCacheSize; }

// WARNING(mainnet)
void DataBaseConfig::setNetwork(Xi::Config::Network::Type network) { this->m_network = network; }

void DataBaseConfig::setCompression(DataBaseConfig::Compression _compression) { compression = _compression; }

namespace Common {
template <>
void toString(const CryptoNote::DataBaseConfig::Compression &compression, std::string &out) {
  using CryptoNote::DataBaseConfig;

  switch (compression) {
    case DataBaseConfig::Compression::None:
      out = "none";
      break;
    case DataBaseConfig::Compression::LZ4:
      out = "lz4";
      break;
    case DataBaseConfig::Compression::LZ4HC:
      out = "lz4hc";
      break;
    default:
      throw std::runtime_error{"Unknown string representation for DataBaseConfig::Compression."};
  }
}
}  // namespace Common

bool serialize(DataBaseConfig::Compression &compression, ISerializer &s) {
  if (s.type() == ISerializer::INPUT) {
    std::string str;
    XI_RETURN_EC_IF_NOT(s.binary(str, ""), false);
    return DataBaseConfig::parseCompression(str, compression);
  } else {
    assert(s.type() == ISerializer::OUTPUT);
    std::string str;
    Common::toString(compression, str);
    return s.binary(str, "");
  }
}
