﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#include "CryptoNoteCore/Transactions/ITransactionBuilder.h"

#include <numeric>
#include <system_error>

#include <Xi/Global.hh>

#include "CryptoNoteCore/CryptoNoteBasic.h"
#include "CryptoNoteCore/Transactions/TransactionApiExtra.h"
#include "CryptoNoteCore/Transactions/TransactionUtils.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"

using namespace Crypto;

namespace CryptoNote {

class TransactionPrefixImpl : public ITransactionReader {
 public:
  TransactionPrefixImpl();
  TransactionPrefixImpl(const TransactionPrefix& prefix, const Hash& transactionHash);

  virtual ~TransactionPrefixImpl() override = default;

  virtual Hash getTransactionHash() const override;
  virtual Hash getTransactionPrefixHash() const override;
  virtual PublicKey getTransactionPublicKey() const override;
  virtual uint64_t getUnlockTime() const override;

  // extra
  virtual bool getPaymentId(PaymentId& paymentId) const override;
  virtual const TransactionExtra& getExtra() const override;

  // inputs
  virtual size_t getInputCount() const override;
  virtual uint64_t getInputTotalAmount() const override;
  virtual TransactionTypes::InputType getInputType(size_t index) const override;
  virtual void getInput(size_t index, KeyInput& input) const override;

  // outputs
  virtual size_t getOutputCount() const override;
  virtual uint64_t getOutputTotalAmount() const override;
  virtual TransactionTypes::OutputTargetType getOutputType(size_t index) const override;
  virtual void getOutput(size_t index, KeyOutput& output, uint64_t& amount) const override;

  // signatures
  virtual size_t getRequiredSignaturesCount(size_t inputIndex) const override;
  virtual bool findOutputsToAccount(const AccountPublicAddress& addr, const SecretKey& viewSecretKey,
                                    std::vector<uint32_t>& outs, uint64_t& outputAmount) const override;

  // various checks
  virtual bool validateInputs() const override;
  virtual bool validateOutputs() const override;
  virtual bool validateSignatures() const override;

  // serialized transaction
  virtual BinaryArray getTransactionData() const override;

  virtual bool getTransactionSecretKey(SecretKey& key) const override;

 private:
  TransactionPrefix m_txPrefix;
  Hash m_txHash;
};

TransactionPrefixImpl::TransactionPrefixImpl() {
}

TransactionPrefixImpl::TransactionPrefixImpl(const TransactionPrefix& prefix, const Hash& transactionHash) {
  m_txPrefix = prefix;
  m_txHash = transactionHash;
}

Hash TransactionPrefixImpl::getTransactionHash() const {
  return m_txHash;
}

Hash TransactionPrefixImpl::getTransactionPrefixHash() const {
  return m_txPrefix.prefixHash();
}

PublicKey TransactionPrefixImpl::getTransactionPublicKey() const {
  return m_txPrefix.extra.publicKey.value_or(Crypto::PublicKey::Null);
}

uint64_t TransactionPrefixImpl::getUnlockTime() const {
  return m_txPrefix.unlockTime;
}

bool TransactionPrefixImpl::getPaymentId(PaymentId& hash) const {
  PaymentId paymentId;
  if (getPaymentIdFromTransactionExtraNonce(m_txPrefix.extra, paymentId)) {
    hash = paymentId;
    return true;
  }
  return false;
}

const TransactionExtra& TransactionPrefixImpl::getExtra() const {
  return m_txPrefix.extra;
}

size_t TransactionPrefixImpl::getInputCount() const {
  return m_txPrefix.inputs.size();
}

uint64_t TransactionPrefixImpl::getInputTotalAmount() const {
  return std::accumulate(m_txPrefix.inputs.begin(), m_txPrefix.inputs.end(), 0ULL,
                         [](uint64_t val, const TransactionInput& in) { return val + getTransactionInputAmount(in); });
}

TransactionTypes::InputType TransactionPrefixImpl::getInputType(size_t index) const {
  return getTransactionInputType(getInputChecked(m_txPrefix, index));
}

void TransactionPrefixImpl::getInput(size_t index, KeyInput& input) const {
  input = std::get<KeyInput>(getInputChecked(m_txPrefix, index, TransactionTypes::InputType::Key));
}

size_t TransactionPrefixImpl::getOutputCount() const {
  return m_txPrefix.outputs.size();
}

uint64_t TransactionPrefixImpl::getOutputTotalAmount() const {
  return getOutputAmount(m_txPrefix);
}

TransactionTypes::OutputTargetType TransactionPrefixImpl::getOutputType(size_t index) const {
  const auto& target = getOutputTargetChecked(getOutputChecked(m_txPrefix, index));
  return getTransactionOutputTargetType(target);
}

void TransactionPrefixImpl::getOutput(size_t index, KeyOutput& output, uint64_t& amount) const {
  const auto& out = getOutputChecked(m_txPrefix, index, TransactionTypes::OutputType::Amount,
                                     TransactionTypes::OutputTargetType::Key);
  const auto& target = getOutputTargetChecked(out);
  amount = std::get<TransactionAmountOutput>(out).amount;
  output = std::get<KeyOutput>(target);
}

size_t TransactionPrefixImpl::getRequiredSignaturesCount(size_t inputIndex) const {
  return ::CryptoNote::getRequiredSignaturesCount(getInputChecked(m_txPrefix, inputIndex));
}

bool TransactionPrefixImpl::findOutputsToAccount(const AccountPublicAddress& addr, const SecretKey& viewSecretKey,
                                                 std::vector<uint32_t>& outs, uint64_t& outputAmount) const {
  return ::CryptoNote::findOutputsToAccount(m_txPrefix, addr, viewSecretKey, outs, outputAmount);
}

bool TransactionPrefixImpl::validateInputs() const {
  return checkInputTypesSupported(m_txPrefix) && checkInputsOverflow(m_txPrefix) &&
         checkInputsKeyimagesDiff(m_txPrefix);
}

bool TransactionPrefixImpl::validateOutputs() const {
  return checkOutsValid(m_txPrefix) && checkOutsOverflow(m_txPrefix);
}

bool TransactionPrefixImpl::validateSignatures() const {
  throw std::system_error(std::make_error_code(std::errc::function_not_supported),
                          "Validating signatures is not supported for transaction prefix");
}

BinaryArray TransactionPrefixImpl::getTransactionData() const {
  return toBinaryArray(m_txPrefix);
}

bool TransactionPrefixImpl::getTransactionSecretKey(SecretKey& key) const {
  XI_UNUSED(key);
  return false;
}

std::unique_ptr<ITransactionReader> createTransactionPrefix(const TransactionPrefix& prefix,
                                                            const Hash& transactionHash) {
  return std::unique_ptr<ITransactionReader>(new TransactionPrefixImpl(prefix, transactionHash));
}

std::unique_ptr<ITransactionReader> createTransactionPrefix(const Transaction& fullTransaction) {
  return std::unique_ptr<ITransactionReader>(new TransactionPrefixImpl(fullTransaction, fullTransaction.prefixHash()));
}

}  // namespace CryptoNote
