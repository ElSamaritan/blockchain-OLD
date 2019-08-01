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
#include <vector>
#include <variant>

#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/OptionalSerialization.hpp>
#include <Serialization/VariantSerialization.hpp>
#include <CryptoNoteCore/CryptoNote.h>
#include <CryptoNoteCore/CryptoNoteSerialization.h>
#include <CryptoNoteCore/Transactions/Transaction.h>

#include "Xi/Blockchain/Explorer/Data/TransactionInfo.hpp"

namespace Xi {
namespace Blockchain {
namespace Explorer {

using TransactionOutput = CryptoNote::TransactionOutput;
using TransactionAmount = CryptoNote::Amount;

using TransactionBaseInput = CryptoNote::BaseInput;

struct DetailedTransactionBaseInput {
  TransactionBaseInput input;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(input)
  KV_END_SERIALIZATION
};

using TransactionKeyInput = CryptoNote::KeyInput;

/// Information about the output used as input.
struct DetailedTransactionKeyInputReference {
  /// The transaction referenced.
  TransactionHash transaction;
  /// The global output index of the used key output in the referenced transaction.
  uint64_t index;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(transaction)
  KV_MEMBER(index)
  KV_END_SERIALIZATION
};
using DetailedTransactionKeyInputReferenceVector = std::vector<DetailedTransactionKeyInputReference>;

/// Information about an input used to transfer coins.
struct DetailedTransactionKeyInput {
  /// Information about the input encoded in the transaction.
  TransactionKeyInput input;
  /// Information about the reference used to spend.
  DetailedTransactionKeyInputReferenceVector references;
  /// Number of inputs mixed.
  uint64_t ring_size;

  XI_PADDING(4)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(input)
  KV_MEMBER(references)
  KV_MEMBER(ring_size)
  KV_END_SERIALIZATION
};

using DetailedTransactionInput = std::variant<DetailedTransactionBaseInput, DetailedTransactionKeyInput>;
using DetailedTransactionInputVector = std::vector<DetailedTransactionInput>;

/// Details about an output in a transaction.
struct DetailedTransactionKeyOutput {
  /// The actual encoded output in the transaction.
  TransactionOutput output;
  /// The global index of the output, only valid once mined.
  std::optional<Transaction::GlobalIndex> global_index;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(output)
  KV_MEMBER(global_index)
  KV_END_SERIALIZATION
};

using DetailedTransactionOutput = std::variant<DetailedTransactionKeyOutput>;
using DetailedTransactionOutputVector = std::vector<DetailedTransactionOutput>;

struct DetailedTransactionInfo {
  /// Lighweighted infos.
  TransactionInfo info;
  /// Inputs used to spend.
  DetailedTransactionInputVector inputs;
  /// Outputs generated.
  DetailedTransactionOutputVector outputs;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(info)
  KV_MEMBER(inputs)
  KV_MEMBER(outputs)
  KV_END_SERIALIZATION
};

using DetailedTransactionInfoVector = std::vector<DetailedTransactionInfo>;

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Explorer::DetailedTransactionInput, 0, 0x01, "base");
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Explorer::DetailedTransactionInput, 1, 0x02, "key");

XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Explorer::DetailedTransactionOutput, 0, 0x01, "key");
