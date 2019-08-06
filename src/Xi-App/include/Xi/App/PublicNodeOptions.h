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

#include <cinttypes>
#include <string>

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>

#include "Xi/App/IOptions.h"

namespace Xi {
namespace App {

struct PublicNodeFees {
  XI_PROPERTY(std::string, address, "")
  XI_PROPERTY(std::string, viewKey, "")
  XI_PROPERTY(uint64_t, amount, 0)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(address(), address)
  KV_MEMBER_RENAME(viewKey(), view_key)
  KV_MEMBER_RENAME(amount(), amount)
  KV_END_SERIALIZATION
};

struct PublicNodeOptions : public IOptions {
  XI_PROPERTY(bool, enabled, true)
  XI_PROPERTY(bool, mining, true)
  XI_PROPERTY(PublicNodeFees, fee)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(enabled(), enabled)
  KV_MEMBER_RENAME(fee(), fee)
  KV_END_SERIALIZATION

  void loadEnvironment(Environment& env) override;
  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;
};

}  // namespace App
}  // namespace Xi
