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

#include "Xi/Blockchain/Transaction/AmountInput.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

bool AmountInput::serialize(CryptoNote::ISerializer& serializer, std::optional<uint16_t> ringSize) {
  XI_RETURN_EC_IF_NOT(serializer(amount, "amount"), false);
  XI_RETURN_EC_IF_NOT(serializer(keyImage, "key_image"), false);

  if (!ringSize) {
    XI_RETURN_EC_IF_NOT(serializer(outputIndices, "output_indices"), false);
  } else {
    if (serializer.isOutput()) {
      XI_RETURN_EC_IF_NOT(outputIndices.size() == *ringSize, false);
    }

    XI_RETURN_EC_IF_NOT(serializer.beginStaticArray(*ringSize, "output_indices"), false);
    if (serializer.isInput()) {
      outputIndices.resize(*ringSize);
    }

    for (auto& index : outputIndices) {
      XI_RETURN_EC_IF_NOT(serializer(index, ""), false);
    }

    XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  }

  XI_RETURN_SC(true);
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
