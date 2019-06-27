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

#include <Serialization/ISerializer.h>
#include <CryptoNoteCore/CryptoNote.h>

namespace Xi {
namespace Blockchain {
namespace Explorer {

struct CurrencyInfo {
  std::string name;
  std::string ticker;
  std::string address_prefix;
  uint64_t total_supply;
  uint64_t premine;
  uint32_t emission_speed;

  std::string homepage;
  std::string description;
  std::string copyright;

  KV_BEGIN_SERIALIZATION

  KV_MEMBER(name)
  KV_MEMBER(ticker)
  KV_MEMBER(address_prefix)
  KV_MEMBER(total_supply)
  KV_MEMBER(premine)
  KV_MEMBER(emission_speed)

  KV_MEMBER(homepage)
  KV_MEMBER(description)
  KV_MEMBER(copyright)

  KV_END_SERIALIZATION
};

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi
