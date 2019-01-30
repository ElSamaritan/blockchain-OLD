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

#include <vector>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include "CryptoNoteCore/CryptoNote.h"
#include "CryptoNoteCore/Transactions/TransactionApiExtra.h"
#include "CryptoNoteCore/Transactions/ITransactionBuilder.h"

namespace CryptoNote {
class TransactionBuilder : public ITransactionBuilder {
 public:
  TransactionBuilder();
  TransactionBuilder(const BinaryArray& txblob);
  TransactionBuilder(const CryptoNote::Transaction& tx);
  ~TransactionBuilder() override = default;

  // ITransactionReader
  virtual Crypto::Hash getTransactionHash() const override;
  virtual Crypto::Hash getTransactionPrefixHash() const override;
  virtual Crypto::PublicKey getTransactionPublicKey() const override;
  virtual uint64_t getUnlockTime() const override;
  virtual bool getPaymentId(Crypto::Hash& hash) const override;
  virtual bool getExtraNonce(BinaryArray& nonce) const override;
  virtual BinaryArray getExtra() const override;

  // inputs
  virtual size_t getInputCount() const override;
  virtual uint64_t getInputTotalAmount() const override;
  virtual TransactionTypes::InputType getInputType(size_t index) const override;
  virtual void getInput(size_t index, KeyInput& input) const override;

  // outputs
  virtual size_t getOutputCount() const override;
  virtual uint64_t getOutputTotalAmount() const override;
  virtual TransactionTypes::OutputType getOutputType(size_t index) const override;
  virtual void getOutput(size_t index, KeyOutput& output, uint64_t& amount) const override;

  virtual size_t getRequiredSignaturesCount(size_t index) const override;
  virtual bool findOutputsToAccount(const AccountPublicAddress& addr, const Crypto::SecretKey& viewSecretKey,
                                    std::vector<uint32_t>& outs, uint64_t& outputAmount) const override;

  // various checks
  virtual bool validateInputs() const override;
  virtual bool validateOutputs() const override;
  virtual bool validateSignatures() const override;

  // get serialized transaction
  virtual BinaryArray getTransactionData() const override;

  // ITransactionWriter

  virtual void setUnlockTime(uint64_t unlockTime) override;
  virtual void setPaymentId(const Crypto::Hash& hash) override;
  virtual void setExtraNonce(const BinaryArray& nonce) override;
  virtual void appendExtra(const BinaryArray& extraData) override;

  // Inputs/Outputs
  virtual size_t addInput(const KeyInput& input) override;
  virtual size_t addInput(const AccountKeys& senderKeys, const TransactionTypes::InputKeyInfo& info,
                          KeyPair& ephKeys) override;

  virtual size_t addOutput(uint64_t amount, const AccountPublicAddress& to) override;
  virtual size_t addOutput(uint64_t amount, const KeyOutput& out) override;

  virtual void signInputKey(size_t input, const TransactionTypes::InputKeyInfo& info, const KeyPair& ephKeys) override;

  // secret key
  virtual bool getTransactionSecretKey(Crypto::SecretKey& key) const override;
  virtual void setTransactionSecretKey(const Crypto::SecretKey& key) override;

 private:
  void invalidateHash();

  std::vector<Crypto::Signature>& getSignatures(size_t input);

  const Crypto::SecretKey& txSecretKey() const {
    if (!secretKey) {
      throw std::runtime_error("Operation requires transaction secret key");
    }
    return *secretKey;
  }

  void checkIfSigning() const {
    if (!transaction.signatures.empty()) {
      throw std::runtime_error("Cannot perform requested operation, since it will invalidate transaction signatures");
    }
  }

  CryptoNote::Transaction transaction;
  boost::optional<Crypto::SecretKey> secretKey;
  mutable boost::optional<Crypto::Hash> transactionHash;
  TransactionExtra extra;
};
}  // namespace CryptoNote
