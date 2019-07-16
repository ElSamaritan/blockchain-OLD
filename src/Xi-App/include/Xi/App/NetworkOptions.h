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

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Config/NetworkType.h>
#include <Xi/Config/Network.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>

#include "Xi/App/IOptions.h"

namespace Xi {
namespace App {
struct NetworkOptions : public IOptions {
  std::string NetworkDir = "./config";
  std::string Network = Xi::Config::Network::defaultNetworkIdentifier();

  // TODO: serialization

  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;
};
}  // namespace App
}  // namespace Xi
