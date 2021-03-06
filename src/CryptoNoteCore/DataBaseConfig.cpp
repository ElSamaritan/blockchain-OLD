﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Develops
//
// Please see the included LICENSE file for more information.

#include "DataBaseConfig.h"

#include <limits>

#include <Xi/Exceptions.hpp>
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
      compression(Compression::LZ4) {
}

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

bool DataBaseConfig::isConfigFolderDefaulted() const {
  return configFolderDefaulted;
}

std::string DataBaseConfig::getDataDir() const {
  return dataDir;
}

uint16_t DataBaseConfig::getBackgroundThreadsCount() const {
  return backgroundThreadsCount;
}

uint32_t DataBaseConfig::getMaxOpenFiles() const {
  return maxOpenFiles;
}

uint64_t DataBaseConfig::getWriteBufferSize() const {
  return writeBufferSize;
}

uint64_t DataBaseConfig::getReadCacheSize() const {
  return readCacheSize;
}

DataBaseConfig::Compression DataBaseConfig::getCompression() const {
  return compression;
}

void DataBaseConfig::setConfigFolderDefaulted(bool defaulted) {
  configFolderDefaulted = defaulted;
}

void DataBaseConfig::setDataDir(const std::string &_dataDir) {
  this->dataDir = _dataDir;
}

void DataBaseConfig::setBackgroundThreadsCount(uint16_t _backgroundThreadsCount) {
  this->backgroundThreadsCount = _backgroundThreadsCount;
}

void DataBaseConfig::setMaxOpenFiles(uint32_t _maxOpenFiles) {
  this->maxOpenFiles = _maxOpenFiles;
}

void DataBaseConfig::setWriteBufferSize(uint64_t _writeBufferSize) {
  this->writeBufferSize = _writeBufferSize;
}

void DataBaseConfig::setReadCacheSize(uint64_t _readCacheSize) {
  this->readCacheSize = _readCacheSize;
}

void DataBaseConfig::setCompression(DataBaseConfig::Compression _compression) {
  compression = _compression;
}

std::string CryptoNote::toString(DataBaseConfig::Compression compression) {
  using CryptoNote::DataBaseConfig;

  switch (compression) {
    case DataBaseConfig::Compression::None:
      return "none";
    case DataBaseConfig::Compression::LZ4:
      return "lz4";
    case DataBaseConfig::Compression::LZ4HC:
      return "lz4hc";
  }
  Xi::exceptional<Xi::InvalidArgumentError>("Unknown compression value: {}", static_cast<uint32_t>(compression));
}
