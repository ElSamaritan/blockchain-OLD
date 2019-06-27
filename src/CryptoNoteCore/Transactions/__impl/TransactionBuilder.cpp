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

#include <Xi/Config/Transaction.h>

#include "CryptoNoteCore/CryptoNoteTools.h"
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

  TransactionExtraPublicKey pk = {txKeys.publicKey};
  extra.set(pk);

  transaction.version = Xi::Config::Transaction::version();
  transaction.unlockTime = 0;
  transaction.extra = extra.serialize();

  secretKey = txKeys.secretKey;
}

TransactionBuilder::TransactionBuilder(const BinaryArray& ba) {
  if (!fromBinaryArray(transaction, ba)) {
    throw std::runtime_error("Invalid transaction data");
  }

  extra.parse(transaction.extra);
  transactionHash = getBinaryArrayHash(ba);  // avoid serialization if we already have blob
}

TransactionBuilder::TransactionBuilder(const CryptoNote::Transaction& tx) : transaction(tx) {
  extra.parse(transaction.extra);
}

void TransactionBuilder::invalidateHash() {
  if (transactionHash.is_initialized()) {
    transactionHash = decltype(transactionHash)();
  }
}

Hash TransactionBuilder::getTransactionHash() const {
  if (!transactionHash.is_initialized()) {
    transactionHash = getObjectHash(transaction);
  }

  return transactionHash.get();
}

Hash TransactionBuilder::getTransactionPrefixHash() const {
  return getObjectHash(*static_cast<const TransactionPrefix*>(&transaction));
}

PublicKey TransactionBuilder::getTransactionPublicKey() const {
  PublicKey pk(PublicKey::Null);
  extra.getPublicKey(pk);
  return pk;
}

uint64_t TransactionBuilder::getUnlockTime() const { return transaction.unlockTime; }

void TransactionBuilder::setUnlockTime(uint64_t unlockTime) {
  checkIfSigning();
  transaction.unlockTime = unlockTime;
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
  const auto& sk = reinterpret_cast<const SecretKey&>(key);
  PublicKey pk;
  PublicKey txPubKey;

  secret_key_to_public_key(sk, pk);
  extra.getPublicKey(txPubKey);

  if (txPubKey != pk) {
    throw std::runtime_error("Secret transaction key does not match public key");
  }

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
  input.amount = info.amount;

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
  TransactionOutput out = {amount, outKey};
  transaction.outputs.emplace_back(out);
  invalidateHash();

  return transaction.outputs.size() - 1;
}

size_t TransactionBuilder::addOutput(uint64_t amount, const KeyOutput& out) {
  checkIfSigning();
  size_t outputIndex = transaction.outputs.size();
  TransactionOutput realOut = {amount, out};
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

  getSignatures(index) = signatures;
  invalidateHash();
}

std::vector<Signature>& TransactionBuilder::getSignatures(size_t input) {
  // update signatures container size if needed
  if (transaction.signatures.size() < transaction.inputs.size()) {
    transaction.signatures.resize(transaction.inputs.size());
  }
  // check range
  if (input >= transaction.signatures.size()) {
    throw std::runtime_error("Invalid input index");
  }

  return transaction.signatures[input];
}

BinaryArray TransactionBuilder::getTransactionData() const { return toBinaryArray(transaction); }

void TransactionBuilder::setPaymentId(const PaymentId& pid) {
  checkIfSigning();
  if (!pid.isValid()) {
    throw std::runtime_error{"invalid payment id"};
  }
  BinaryArray paymentIdBlob;
  setPaymentIdToTransactionExtraNonce(paymentIdBlob, pid);
  setExtraNonce(paymentIdBlob);
}

bool TransactionBuilder::getPaymentId(PaymentId& pid) const {
  BinaryArray nonce;
  if (getExtraNonce(nonce)) {
    PaymentId paymentId;
    if (getPaymentIdFromTransactionExtraNonce(nonce, paymentId)) {
      pid = paymentId;
      return true;
    }
  }
  return false;
}

void TransactionBuilder::setExtraNonce(const BinaryArray& nonce) {
  checkIfSigning();
  TransactionExtraNonce extraNonce = {nonce};
  extra.set(extraNonce);
  transaction.extra = extra.serialize();
  invalidateHash();
}

void TransactionBuilder::appendExtra(const BinaryArray& extraData) {
  checkIfSigning();
  transaction.extra.insert(transaction.extra.end(), extraData.begin(), extraData.end());
}

bool TransactionBuilder::getExtraNonce(BinaryArray& nonce) const {
  TransactionExtraNonce extraNonce;
  if (extra.get(extraNonce)) {
    nonce = extraNonce.nonce;
    return true;
  }
  return false;
}

BinaryArray TransactionBuilder::getExtra() const { return transaction.extra; }

size_t TransactionBuilder::getInputCount() const { return transaction.inputs.size(); }

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

size_t TransactionBuilder::getOutputCount() const { return transaction.outputs.size(); }

uint64_t TransactionBuilder::getOutputTotalAmount() const {
  return std::accumulate(transaction.outputs.begin(), transaction.outputs.end(), 0ULL,
                         [](uint64_t val, const TransactionOutput& out) { return val + out.amount; });
}

TransactionTypes::OutputType TransactionBuilder::getOutputType(size_t index) const {
  return getTransactionOutputType(getOutputChecked(transaction, index).target);
}

void TransactionBuilder::getOutput(size_t index, KeyOutput& output, uint64_t& amount) const {
  const auto& out = getOutputChecked(transaction, index, TransactionTypes::OutputType::Key);
  output = std::get<KeyOutput>(out.target);
  amount = out.amount;
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
  if (transaction.signatures.size() < transaction.inputs.size()) {
    return false;
  }

  for (size_t i = 0; i < transaction.inputs.size(); ++i) {
    if (getRequiredSignaturesCount(i) > transaction.signatures[i].size()) {
      return false;
    }
  }

  return true;
}
}  // namespace CryptoNote
