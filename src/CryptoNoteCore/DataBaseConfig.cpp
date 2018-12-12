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

DataBaseConfig::DataBaseConfig()
    : dataDir(Tools::getDefaultDataDirectory()),
      backgroundThreadsCount(CryptoNote::Config::Database::backgroundThreads()),
      maxOpenFiles(CryptoNote::Config::Database::maximumOpenFiles()),
      writeBufferSize(CryptoNote::Config::Database::writeBufferSize()),
      readCacheSize(CryptoNote::Config::Database::readBufferSize()),
      testnet(false) {}

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
