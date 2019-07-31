/* ============================================================================================== *
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

#pragma once

#include <string>
#include <cinttypes>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <boost/optional.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Byte.hh>
#include <Xi/Config/NetworkType.h>
#include <Common/Util.h>
#include <CryptoNoteCore/DataBaseConfig.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>

#include "Xi/App/IOptions.h"

namespace Xi {
namespace App {
struct DatabaseOptions : public IOptions {
  std::string DataDirectory = Tools::getDefaultDataDirectory();
  uint16_t Threads = 1;
  uint16_t MaximumOpenFiles = 16;
  uint64_t WriteBufferSize = 32_MB;
  uint64_t ReadCacheSize = 128_MB;
  CryptoNote::DataBaseConfig::Compression Compression = CryptoNote::DataBaseConfig::Compression::LZ4;
  bool LightNode = false;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(DataDirectory, data_dir)
  KV_MEMBER_RENAME(Threads, threads)
  KV_MEMBER_RENAME(MaximumOpenFiles, max_open_files)
  KV_MEMBER_RENAME(WriteBufferSize, write_buffer_size)
  KV_MEMBER_RENAME(ReadCacheSize, read_cache_size)
  KV_MEMBER_RENAME(Compression, compression)
  KV_MEMBER_RENAME(LightNode, light_node)
  KV_END_SERIALIZATION

  void loadEnvironment(Environment& env) override;
  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;

  CryptoNote::DataBaseConfig getConfig() const;
};
}  // namespace App
}  // namespace Xi
