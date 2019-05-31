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

#include "Xi/Blockchain/Block/Header.hpp"

bool Xi::Blockchain::Block::Header::serialize(CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer(majorVersion, "major_version"), false);
  XI_RETURN_EC_IF_NOT(serializer(minorVersion, "minor_version"), false);
  XI_RETURN_EC_IF_NOT(serializer(nonce, "nonce"), false);
  XI_RETURN_EC_IF_NOT(serializer(features, "features"), false);
  XI_RETURN_EC_IF_NOT(serializer(timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializer(previousBlockHash, "previous_block_hash"), false);
  return true;
}
