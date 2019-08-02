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

#include "CryptoNoteCore/Transactions/__impl/TransactionBuilder.h"

#include <algorithm>
#include <numeric>

#include <Xi/Exceptions.hpp>
#include <Xi/Config/Transaction.h>

#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteCore/Transactions/TransactionUtils.h"

using namespace Crypto;

namespace {

using namespace CryptoNote;

void derivePublicKey(const AccountPublicAddress& to, const SecretKey& txKey, size_t outputIndex,
                     PublicKey& ephemeralKey) {
  KeyDerivation derivation;
  generate_key_derivation(to.viewPublicKey, txKey, derivation);
  derive_public_key(derivation, outputIndex, to.spendPublicKey, ephemeralKey);
}

}  // namespace

namespace CryptoNote {

TransactionBuilder::TransactionBuilder() {
  CryptoNote::KeyPair txKeys(CryptoNote::generateKeyPair());

  transaction.version = 1;
  transaction.type = TransactionType::Transfer;
  transaction.unlockTime = 0;
  transaction.signatures = TransactionSignatureCollection{};

  setTransactionPublicKeyToExtra(transaction.extra, txKeys.publicKey);
  secretKey = txKeys.secretKey;
}

TransactionBuilder::TransactionBuilder(const BinaryArray& ba) {
  if (!fromBinaryArray(transaction, ba)) {
    throw std::runtime_error("Invalid transaction data");
  }
}

TransactionBuilder::TransactionBuilder(const CryptoNote::Transaction& tx) : transaction(tx) {
  /* */
}

void TransactionBuilder::invalidateHash() {
  if (transactionHash.is_initialized()) {
    transactionHash = decltype(transactionHash)();
  }
  if (transactionPrefixHash.is_initialized()) {
    transactionPrefixHash = decltype(transactionPrefixHash)();
  }
}

Hash TransactionBuilder::getTransactionHash() const {
  if (!transactionHash.is_initialized()) {
    transactionHash = transaction.hash();
  }

  return transactionHash.get();
}

Hash TransactionBuilder::getTransactionPrefixHash() const {
  if (!transactionPrefixHash.is_initialized()) {
    transactionPrefixHash = transaction.prefixHash();
  }
  return transactionPrefixHash.get();
}

PublicKey TransactionBuilder::getTransactionPublicKey() const {
  return getTransactionPublicKeyFromExtra(transaction.extra);
}

uint64_t TransactionBuilder::getUnlockTime() const {
  return transaction.unlockTime;
}

void TransactionBuilder::setUnlockTime(uint64_t unlockTime) {
  checkIfSigning();
  transaction.unlockTime = unlockTime;
  transaction.features |= TransactionFeature::UniformUnlock;
  invalidateHash();
}

bool TransactionBuilder::getTransactionSecretKey(SecretKey& key) const {
  if (!secretKey) {
    return false;
  }
  key = reinterpret_cast<const SecretKey&>(secretKey.get());
  return true;
}

void TransactionBuilder::setTransactionSecretKey(const SecretKey& key) {
  checkIfSigning();
  Xi::exceptional_if_not<Xi::InvalidArgumentError>(key.isValid(), "invalid transaction secret key: {}", key.toString());

  setTransactionPublicKeyToExtra(transaction.extra, key.toPublicKey());
  secretKey = key;
}

size_t TransactionBuilder::addInput(const KeyInput& input) {
  checkIfSigning();
  transaction.inputs.emplace_back(input);
  invalidateHash();
  return transaction.inputs.size() - 1;
}

size_t TransactionBuilder::addInput(const AccountKeys& senderKeys, const TransactionTypes::InputKeyInfo& info,
                                    KeyPair& ephKeys) {
  checkIfSigning();
  KeyInput input;
  input.amount = CanonicalAmount{info.amount};

  generate_key_image_helper(senderKeys, info.realOutput.transactionPublicKey, info.realOutput.outputInTransaction,
                            ephKeys, input.keyImage);

  // fill outputs array and use relative offsets
  for (const auto& out : info.outputs) {
    input.outputIndices.push_back(out.outputIndex);
  }

  input.outputIndices = absolute_output_offsets_to_relative(input.outputIndices);
  return addInput(input);
}

size_t TransactionBuilder::addOutput(uint64_t amount, const AccountPublicAddress& to) {
  checkIfSigning();

  KeyOutput outKey;
  derivePublicKey(to, txSecretKey(), transaction.outputs.size(), outKey.key);
  TransactionOutput out = TransactionAmountOutput{CanonicalAmount{amount}, outKey};
  transaction.outputs.emplace_back(out);
  invalidateHash();

  return transaction.outputs.size() - 1;
}

size_t TransactionBuilder::addOutput(uint64_t amount, const KeyOutput& out) {
  checkIfSigning();
  size_t outputIndex = transaction.outputs.size();
  TransactionOutput realOut = TransactionAmountOutput{CanonicalAmount{amount}, out};
  transaction.outputs.emplace_back(realOut);
  invalidateHash();
  return outputIndex;
}

void TransactionBuilder::signInputKey(size_t index, const TransactionTypes::InputKeyInfo& info,
                                      const KeyPair& ephKeys) {
  const auto& input = std::get<KeyInput>(getInputChecked(transaction, index, TransactionTypes::InputType::Key));
  Hash prefixHash = getTransactionPrefixHash();

  std::vector<Signature> signatures;
  std::vector<const PublicKey*> keysPtrs;

  for (const auto& o : info.outputs) {
    keysPtrs.push_back(reinterpret_cast<const PublicKey*>(&o.targetKey));
  }

  signatures.resize(keysPtrs.size());

  generate_ring_signature(reinterpret_cast<const Hash&>(prefixHash), reinterpret_cast<const KeyImage&>(input.keyImage),
                          keysPtrs, reinterpret_cast<const SecretKey&>(ephKeys.secretKey),
                          info.realOutput.transactionIndex, signatures.data());

  getRingSignature(index) = signatures;
  invalidateHash();
}

TransactionRingSignature& TransactionBuilder::getRingSignature(size_t input) {
  if (!transaction.signatures.has_value() ||
      !std::holds_alternative<TransactionSignatureCollection>(*transaction.signatures)) {
    throw std::runtime_error{"invalid signature type"};
  }
  auto& signatures = std::get<TransactionSignatureCollection>(*transaction.signatures);
  // update signatures container size if needed
  if (signatures.size() < transaction.inputs.size()) {
    signatures.resize(transaction.inputs.size());
  }
  // check range
  if (input >= signatures.size()) {
    throw std::runtime_error("Invalid input index");
  }

  if (!std::holds_alternative<TransactionRingSignature>(signatures[input])) {
    throw std::runtime_error{"invalid signature type"};
  }

  return std::get<TransactionRingSignature>(signatures[input]);
}

const SecretKey& TransactionBuilder::txSecretKey() const {
  if (!secretKey) {
    throw std::runtime_error("Operation requires transaction secret key");
  }
  return *secretKey;
}

void TransactionBuilder::checkIfSigning() const {
  if (!transaction.signatures.has_value()) {
    return;
  }

  if (!std::holds_alternative<TransactionSignatureCollection>(*transaction.signatures)) {
    throw std::runtime_error{"invalid signature type"};
  }

  if (!std::get<TransactionSignatureCollection>(*transaction.signatures).empty()) {
    throw std::runtime_error("Cannot perform requested operation, since it will invalidate transaction signatures");
  }
}

BinaryArray TransactionBuilder::getTransactionData() const {
  return toBinaryArray(transaction);
}

void TransactionBuilder::setPaymentId(const PaymentId& pid) {
  checkIfSigning();
  if (!pid.isValid()) {
    throw std::runtime_error{"invalid payment id"};
  }
  setPaymentIdToTransactionExtraNonce(transaction.extra, pid);
}

bool TransactionBuilder::getPaymentId(PaymentId& pid) const {
  return getPaymentIdFromTransactionExtraNonce(transaction.extra, pid);
}

const TransactionExtra& TransactionBuilder::getExtra() const {
  return transaction.extra;
}

void TransactionBuilder::applyExtra(const TransactionExtra& extra) {
  if (extra.publicKey) {
    throw std::runtime_error{"extra public key cannot be applied, use setSecretKey"};
  }
  if (extra.paymentId) {
    setPaymentId(*extra.paymentId);
  }
}

size_t TransactionBuilder::getInputCount() const {
  return transaction.inputs.size();
}

uint64_t TransactionBuilder::getInputTotalAmount() const {
  return std::accumulate(transaction.inputs.begin(), transaction.inputs.end(), 0ULL,
                         [](uint64_t val, const TransactionInput& in) { return val + getTransactionInputAmount(in); });
}

TransactionTypes::InputType TransactionBuilder::getInputType(size_t index) const {
  return getTransactionInputType(getInputChecked(transaction, index));
}

void TransactionBuilder::getInput(size_t index, KeyInput& input) const {
  input = std::get<KeyInput>(getInputChecked(transaction, index, TransactionTypes::InputType::Key));
}

size_t TransactionBuilder::getOutputCount() const {
  return transaction.outputs.size();
}

uint64_t TransactionBuilder::getOutputTotalAmount() const {
  return getOutputAmount(transaction);
}

TransactionTypes::OutputTargetType TransactionBuilder::getOutputType(size_t index) const {
  return getTransactionOutputTargetType(getOutputTargetChecked(getOutputChecked(transaction, index)));
}

void TransactionBuilder::getOutput(size_t index, KeyOutput& output, uint64_t& amount) const {
  const auto& out = getOutputChecked(transaction, index, TransactionTypes::OutputType::Amount,
                                     TransactionTypes::OutputTargetType::Key);
  const auto& target = getOutputTargetChecked(out);

  amount = std::get<TransactionAmountOutput>(out).amount;
  output = std::get<KeyOutput>(target);
}

bool TransactionBuilder::findOutputsToAccount(const AccountPublicAddress& addr, const SecretKey& viewSecretKey,
                                              std::vector<uint32_t>& out, uint64_t& amount) const {
  return ::CryptoNote::findOutputsToAccount(transaction, addr, viewSecretKey, out, amount);
}

size_t TransactionBuilder::getRequiredSignaturesCount(size_t index) const {
  return ::getRequiredSignaturesCount(getInputChecked(transaction, index));
}

bool TransactionBuilder::validateInputs() const {
  return checkInputTypesSupported(transaction) && checkInputsOverflow(transaction) &&
         checkInputsKeyimagesDiff(transaction);
}

bool TransactionBuilder::validateOutputs() const {
  return checkOutsValid(transaction) && checkOutsOverflow(transaction);
}

bool TransactionBuilder::validateSignatures() const {
  XI_RETURN_EC_IF_NOT(transaction.signatures, false);
  XI_RETURN_EC_IF_NOT(std::holds_alternative<TransactionSignatureCollection>(*transaction.signatures), false);
  const auto& signatures = std::get<TransactionSignatureCollection>(*transaction.signatures);
  XI_RETURN_EC_IF(signatures.size() < transaction.inputs.size(), false);
  for (size_t i = 0; i < transaction.inputs.size(); ++i) {
    XI_RETURN_EC_IF_NOT(std::holds_alternative<TransactionRingSignature>(signatures[i]), false);
    const auto& ringSignature = std::get<TransactionRingSignature>(signatures[i]);
    XI_RETURN_EC_IF(getRequiredSignaturesCount(i) > ringSignature.size(), false);
  }
  XI_RETURN_SC(true);
}
}  // namespace CryptoNote
