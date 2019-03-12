/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Utils/Conversion.h>
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

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(DataDirectory)
  KV_MEMBER(Threads)
  KV_MEMBER(MaximumOpenFiles)
  KV_MEMBER(WriteBufferSize)
  KV_MEMBER(ReadCacheSize)
  KV_MEMBER(Compression)
  KV_END_SERIALIZATION

  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;

  CryptoNote::DataBaseConfig getConfig(Xi::Config::Network::Type network) const;
};
}  // namespace App
}  // namespace Xi
