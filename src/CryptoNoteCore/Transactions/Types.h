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

#include <Xi/Crypto/PublicKey.hpp>

namespace CryptoNote {

namespace TransactionTypes {

enum class InputType : uint8_t { Invalid, Key, Generating };
enum class OutputType : uint8_t { Invalid, Key };

struct GlobalOutput {
  Crypto::PublicKey targetKey;
  uint32_t outputIndex;
};

typedef std::vector<GlobalOutput> GlobalOutputsContainer;

struct OutputKeyInfo {
  Crypto::PublicKey transactionPublicKey;
  size_t transactionIndex;
  size_t outputInTransaction;
};

struct InputKeyInfo {
  uint64_t amount;
  GlobalOutputsContainer outputs;
  OutputKeyInfo realOutput;
};
}  // namespace TransactionTypes

}  // namespace CryptoNote
