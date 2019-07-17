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

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>

#include "Xi/Config/VersionContainer.hpp"

namespace Xi {
namespace Config {

/*!
 * \brief Configurates a static reward for each block, for a built in address.
 *
 * As an alternative to a genesis block reward you can use this config to enable a built in reward system that generates
 * rewards over the lifetime of the blockchain for a fixed address. The benefit of this approach is an increased level
 * of trust for your community. First your address will not have access to all generated coins from the beginning
 * which prevents an so called 'exit scam', second if the community is not satisfied by the development and/or the
 * usage of the reward they are able to fork the reward out and continue the development of the blockchain without
 * paying out the initial address any longer.
 *
 */
namespace StaticReward {

class Configuration {
 public:
  XI_PROPERTY(uint64_t, amount)
  XI_PROPERTY(std::string, address)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(amount(), amount)
  KV_MEMBER_RENAME(address(), address)
  KV_END_SERIALIZATION

  bool isEnabled() const {
    return !this->address().empty();
  }
};

using Container = VersionContainer<StaticReward::Configuration>;

}  // namespace StaticReward
}  // namespace Config
}  // namespace Xi
