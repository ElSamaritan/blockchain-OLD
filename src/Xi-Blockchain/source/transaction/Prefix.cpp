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

#include "Xi/Blockchain/Transaction/Prefix.hpp"

#include <Serialization/SerializationOverloads.h>
#include <CryptoNoteCore/CryptoNoteTools.h>

namespace Xi {
namespace Blockchain {
namespace Transaction {

bool Prefix::serialize(CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer(version, "version"), false);
  XI_RETURN_SC_IF(version == 0, true);
  XI_RETURN_EC_IF_NOT(serializer(unlockTime, "unlock_time"), false);
  XI_RETURN_EC_IF_NOT(serializer(inputs, "inputs"), false);
  XI_RETURN_EC_IF_NOT(serializer(outputs, "outputs"), false);
  XI_RETURN_EC_IF_NOT(serializer.binary(extra, "extra"), false);
  XI_RETURN_SC(true);
}

Crypto::FastHash Prefix::prefixHash() const {
  return Crypto::FastHash::computeObjectHash<Prefix>(*this).takeOrThrow();
}

uint64_t Prefix::prefixBinarySize() const {
  return CryptoNote::toBinaryArray<Prefix>(*this).size();
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
