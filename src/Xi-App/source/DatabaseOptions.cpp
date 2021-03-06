﻿/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include "Xi/App/DatabaseOptions.h"

#include <Xi/Global.hh>
#include <Xi/Exceptional.hpp>
#include <Xi/FileSystem.h>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(DatabaseOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InsufficientThreads, "database requires at least 1 thread.", DatabaseOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InsufficientWriteBuffer, "database requires at least 4MB write buffer.", DatabaseOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InsufficientReadCache, "database requires at least 16 MB read cache.", DatabaseOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidCompression, "invalid database compression, use none, lz4 or lz4hc.",
                                DatabaseOption)
}  // namespace

void Xi::App::DatabaseOptions::loadEnvironment(Xi::App::Environment &env) {
  std::string compression{};
  // clang-format off
  env
    (DataDirectory, "DATA_DIR")
    (Threads, "DB_THREADS")
    (WriteBufferSize, "DB_WRITE_BUFFER")
    (ReadCacheSize, "DB_READ_BUFFER")
    (compression, "DB_COMPRESSION")
    (LightNode, "LIGHT_NODE")
  ;
  // clang-format on
  if (!compression.empty()) {
    if (!CryptoNote::DataBaseConfig::parseCompression(compression, Compression)) {
      exceptional<InvalidCompressionError>();
    }
  }
}

void Xi::App::DatabaseOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("core")
    ("data-dir", "directory to store chain data, defaulted to data directory if not provided",
        cxxopts::value<std::string>(DataDirectory)->default_value(DataDirectory), "directory path")

    ("light-node", "prunes transaction signatures to sparse memory footprint",
        cxxopts::value<bool>()->default_value(LightNode ? "true" : "false")
                              ->implicit_value("true"))
  ;

  options.add_options("database")
    ("db-threads", "maximum threads utilized to read/write the database",
        cxxopts::value<uint16_t>(Threads)->default_value(std::to_string(Threads)), "# > 0")

    ("db-write-buffer", "maximum buffer allocated for a write batch",
        cxxopts::value<uint64_t>(WriteBufferSize)->default_value(std::to_string(WriteBufferSize)), "bytes size")

    ("db-read-buffer", "maximum cache size allocated for reoccuring reads",
        cxxopts::value<uint64_t>(ReadCacheSize)->default_value(std::to_string(ReadCacheSize)), "bytes size")

    ("db-compression", "compression used to minimize database size",
        cxxopts::value<std::string>()->default_value(toString(Compression)), "none|lz4|lz4hc")
  ;
  // clang-format on
}

bool Xi::App::DatabaseOptions::evaluateParsedOptions(const cxxopts::Options &options,
                                                     const cxxopts::ParseResult &result) {
  XI_UNUSED(options);
  if (Threads < 1) {
    exceptional<InsufficientThreadsError>();
  }
  if (WriteBufferSize < 4_MB) {
    exceptional<InsufficientWriteBufferError>();
  }
  if (ReadCacheSize < 15_MB) {
    exceptional<InsufficientReadCacheError>();
  }
  if (result.count("db-compression")) {
    if (!CryptoNote::DataBaseConfig::parseCompression(result["db-compression"].as<std::string>(), Compression)) {
      exceptional<InvalidCompressionError>();
    }
  }
  if (result.count("light-node") > 0) {
    LightNode = result["light-node"].as<bool>();
  }
  FileSystem::ensureDirectoryExists(DataDirectory).throwOnError();
  return false;
}

CryptoNote::DataBaseConfig Xi::App::DatabaseOptions::getConfig() const {
  CryptoNote::DataBaseConfig config;
  config.setDataDir(DataDirectory);
  config.setCompression(Compression);
  config.setMaxOpenFiles(MaximumOpenFiles);
  config.setReadCacheSize(ReadCacheSize);
  config.setWriteBufferSize(WriteBufferSize);
  config.setBackgroundThreadsCount(Threads);
  return config;
}
