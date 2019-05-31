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

#include <cinttypes>
#include <vector>
#include <variant>

#include <Xi/ExternalIncludePush.h>
#include <boost/variant.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Blockchain/Block/Height.hpp>
#include <Serialization/VariantSerialization.hpp>
#include <crypto/CryptoTypes.h>

namespace CryptoNote {

struct BaseInput {
  /// Index of the block generating coins.
  Xi::Blockchain::Block::Height height;
};

struct KeyInput {
  /// Amount of the used input, all used outputs must correspond to this amount.
  uint64_t amount;

  /// Delta encoded global indices of outputs used (real one + mixins)
  std::vector<uint32_t> outputIndices;

  /// Double spending protection.
  Crypto::KeyImage keyImage;
};

struct KeyOutput {
  /// Diffie Hellmann key exchange, derived from the epheremal keys and destination public key.
  Crypto::PublicKey key;
};

typedef std::variant<BaseInput, KeyInput> TransactionInput;
typedef std::variant<KeyOutput> TransactionOutputTarget;

struct TransactionOutput {
  uint64_t amount;
  TransactionOutputTarget target;
};

struct TransactionPrefix {
  uint8_t version;
  uint64_t unlockTime;
  std::vector<TransactionInput> inputs;
  std::vector<TransactionOutput> outputs;
  std::vector<uint8_t> extra;
};

struct Transaction : public TransactionPrefix {
  static const Transaction Null;

  std::vector<std::vector<Crypto::Signature>> signatures;

  Transaction();

  bool isNull() const;
  void nullify();
};

struct BaseTransaction : public TransactionPrefix {};
}  // namespace CryptoNote

XI_SERIALIZATION_VARIANT_TAG(CryptoNote::TransactionInput, 0, 0x01, "base_input")
XI_SERIALIZATION_VARIANT_TAG(CryptoNote::TransactionInput, 1, 0x02, "key_input")

XI_SERIALIZATION_VARIANT_TAG(CryptoNote::TransactionOutputTarget, 0, 0x01, "key_output")
