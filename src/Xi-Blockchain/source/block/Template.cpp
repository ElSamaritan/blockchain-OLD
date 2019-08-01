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

#include "Xi/Blockchain/Block/Template.hpp"

#include <Serialization/SerializationOverloads.h>

namespace Xi {
namespace Blockchain {
namespace Block {

bool Xi::Blockchain::Block::Template::serialize(CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(Header::serialize(serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(baseTransaction, "base_transaction"), false);
  XI_RETURN_EC_IF_NOT(
      serializer.optional(hasFlag(features, Feature::Transactions), transactionHashes, "transaction_hashes"), false);
  XI_RETURN_SC(true);
}

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi
