// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "BlockchainExplorerDataSerialization.h"

#include <stdexcept>

#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "CryptoNoteCore/CryptoNoteSerialization.h"

#include "Serialization/SerializationOverloads.h"

namespace CryptoNote {

namespace {

template <typename T>
[[nodiscard]] bool serializePod(T& v, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializer.binary(&v, sizeof(v), name);
}

}  // namespace

// namespace CryptoNote {

[[nodiscard]] bool serialize(TransactionOutputDetails& output, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(output.output, "output"), false);
  XI_RETURN_EC_IF_NOT(serializer(output.globalIndex, "global_index"), false);
  return true;
}

[[nodiscard]] bool serialize(TransactionOutputReferenceDetails& outputReference, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializePod(outputReference.transactionHash, "transaction_hash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(outputReference.number, "number"), false);
  return true;
}

[[nodiscard]] bool serialize(BaseInputDetails& inputBase, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(inputBase.input, "input"), false);
  XI_RETURN_EC_IF_NOT(serializer(inputBase.amount, "amount"), false);
  return true;
}

[[nodiscard]] bool serialize(KeyInputDetails& inputToKey, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(inputToKey.input, "input"), false);
  XI_RETURN_EC_IF_NOT(serializer(inputToKey.mixin, "mixin"), false);
  XI_RETURN_EC_IF_NOT(serializer(inputToKey.output, "output"), false);
  return true;
}

[[nodiscard]] bool serialize(TransactionExtraDetails& extra, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializePod(extra.publicKey, "public_key", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(extra.nonce, "nonce"), false);
  XI_RETURN_EC_IF_NOT(serializer.binary(extra.raw, "raw"), false);
  return true;
}

[[nodiscard]] bool serialize(TransactionDetails& transaction, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializePod(transaction.hash, "hash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.size, "size"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.fee, "fee"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.totalInputsAmount, "total_inputs_amount"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.totalOutputsAmount, "total_outputs_amount"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.mixin, "mixin"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.unlockTime, "unlockTime"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializePod(transaction.paymentId, "paymentId", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.inBlockchain, "in_blockchain"), false);
  XI_RETURN_EC_IF_NOT(serializePod(transaction.blockHash, "block_hash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.blockHeight, "block_height"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.extra, "extra"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.inputs, "inputs"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.outputs, "outputs"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.signatures, "signatures"), false);
  XI_RETURN_SC(true);
}

[[nodiscard]] bool serialize(BlockDetails& block, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(block.version, "version"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.upgradeVote, "upgrade_vote"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializePod(block.prevBlockHash, "prevBlockHash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(block.nonce, "nonce"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.height, "height"), false);
  XI_RETURN_EC_IF_NOT(serializePod(block.hash, "hash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(block.difficulty, "difficulty"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.reward, "reward"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.baseReward, "base_reward"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.staticReward, "static_reward"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.blockSize, "block_size"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.transactionsCumulativeSize, "transactions_cumulative_size"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.alreadyGeneratedCoins, "already_generated_coins"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.alreadyGeneratedTransactions, "already_generated_transactions"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.sizeMedian, "sizeMedian"), false);
  /* Some serializers don't support doubles, which causes this to fail and
     not serialize the whole object
  serializer(block.penalty, "penalty");
  */
  XI_RETURN_EC_IF_NOT(serializer(block.totalFeeAmount, "total_fee_amount"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.transactions, "transactions"), false);
  return true;
}

}  // namespace CryptoNote
