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
#include <config/CryptoNoteConfig.h>

using namespace CryptoNote;

namespace {
const uint64_t MEGABYTE = 1024 * 1024;
}

DataBaseConfig::DataBaseConfig()
    : dataDir(Tools::getDefaultDataDirectory()),
      backgroundThreadsCount(DATABASE_DEFAULT_BACKGROUND_THREADS_COUNT),
      maxOpenFiles(DATABASE_DEFAULT_MAX_OPEN_FILES),
      writeBufferSize(DATABASE_WRITE_BUFFER_DEFAULT_SIZE),
      readCacheSize(DATABASE_READ_BUFFER_DEFAULT_SIZE),
      testnet(false) {}

bool DataBaseConfig::init(const std::string dataDirectory, const int backgroundThreads, const int openFiles,
                          const int writeBuffer, const int readCache) {
  dataDir = dataDirectory;

  if (backgroundThreads < std::numeric_limits<uint16_t>::min() ||
      backgroundThreads > std::numeric_limits<uint16_t>::max())
    return false;
  backgroundThreadsCount = static_cast<uint16_t>(backgroundThreads);

  if (openFiles < 0) return false;
  maxOpenFiles = static_cast<uint32_t>(openFiles);

  if (writeBuffer < 0) return false;
  writeBufferSize = static_cast<uint64_t>(writeBuffer) * MEGABYTE;

  if (readCache < 0) return false;
  readCacheSize = static_cast<uint64_t>(readCache) * MEGABYTE;

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

bool DataBaseConfig::getTestnet() const { return testnet; }

void DataBaseConfig::setConfigFolderDefaulted(bool defaulted) { configFolderDefaulted = defaulted; }

void DataBaseConfig::setDataDir(const std::string& _dataDir) { this->dataDir = _dataDir; }

void DataBaseConfig::setBackgroundThreadsCount(uint16_t _backgroundThreadsCount) {
  this->backgroundThreadsCount = _backgroundThreadsCount;
}

void DataBaseConfig::setMaxOpenFiles(uint32_t _maxOpenFiles) { this->maxOpenFiles = _maxOpenFiles; }

void DataBaseConfig::setWriteBufferSize(uint64_t _writeBufferSize) { this->writeBufferSize = _writeBufferSize; }

void DataBaseConfig::setReadCacheSize(uint64_t _readCacheSize) { this->readCacheSize = _readCacheSize; }

// WARNING(mainnet)
void DataBaseConfig::setTestnet(bool _testnet) { this->testnet = _testnet; }
