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

#pragma once

#include <cstdint>
#include <unordered_map>
#include <mutex>
#include <vector>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "crypto/crypto.h"
#include "CryptoNoteCore/CryptoNoteBasic.h"
#include "CryptoNoteCore/CryptoNoteSerialization.h"
#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/Blockchain/BlockHeight.hpp"
#include "Logging/LoggerRef.h"
#include "Serialization/ISerializer.h"
#include "Serialization/SerializationOverloads.h"

#include "CryptoNoteCore/Transactions/ITransactionBuilder.h"
#include "Transfers/ITransfersContainer.h"

namespace CryptoNote {

struct TransactionOutputInformationIn;

class SpentOutputDescriptor {
 public:
  SpentOutputDescriptor();
  SpentOutputDescriptor(const TransactionOutputInformationIn& transactionInfo);
  SpentOutputDescriptor(const Crypto::KeyImage* keyImage);

  void assign(const Crypto::KeyImage* keyImage);

  bool isValid() const;

  bool operator==(const SpentOutputDescriptor& other) const;
  size_t hash() const;

 private:
  TransactionTypes::OutputType m_type;
  union {
    const Crypto::KeyImage* m_keyImage;
    struct {
      uint64_t m_amount;
      uint32_t m_globalOutputIndex;
    } m_data;
  };
};

struct SpentOutputDescriptorHasher {
  size_t operator()(const SpentOutputDescriptor& descriptor) const { return descriptor.hash(); }
};

struct TransactionOutputInformationIn : public TransactionOutputInformation {
  Crypto::KeyImage keyImage;  //!< \attention Used only for TransactionTypes::OutputType::Key
};

struct TransactionOutputInformationEx : public TransactionOutputInformationIn {
  uint64_t unlockTime;
  BlockHeight blockHeight;
  uint32_t transactionIndex;
  bool visible;

  SpentOutputDescriptor getSpentOutputDescriptor() const { return SpentOutputDescriptor(*this); }
  const Crypto::Hash& getTransactionHash() const { return transactionHash; }

  KV_BEGIN_SERIALIZATION
  XI_RETURN_EC_IF_NOT(s(reinterpret_cast<uint8_t&>(type), "type"), false);
  KV_MEMBER(amount)
  KV_MEMBER_RENAME(globalOutputIndex, global_output_index)
  KV_MEMBER_RENAME(outputInTransaction, output_in_transaction)
  KV_MEMBER_RENAME(transactionPublicKey, transaction_public_key)
  KV_MEMBER_RENAME(keyImage, key_image)
  KV_MEMBER_RENAME(unlockTime, unlock_time)
  KV_MEMBER_RENAME(blockHeight, block_height)
  KV_MEMBER_RENAME(blockHeight, block_height)
  KV_MEMBER_RENAME(transactionIndex, transaction_index)
  KV_MEMBER_RENAME(transactionHash, transaction_hash)
  KV_MEMBER(visible)

  if (type == TransactionTypes::OutputType::Key) {
    KV_MEMBER_RENAME(outputKey, output_key)
  }
  KV_END_SERIALIZATION
};

struct TransactionBlockInfo {
  BlockHeight height;
  uint64_t timestamp;
  uint32_t transactionIndex;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(height)
  KV_MEMBER(timestamp)
  KV_MEMBER_RENAME(transactionIndex, transaction_index)
  KV_END_SERIALIZATION
};

struct SpentTransactionOutput : TransactionOutputInformationEx {
  TransactionBlockInfo spendingBlock;
  Crypto::Hash spendingTransactionHash;
  uint32_t inputInTransaction;

  const Crypto::Hash& getSpendingTransactionHash() const { return spendingTransactionHash; }

  KV_BEGIN_SERIALIZATION
  KV_BASE(TransactionOutputInformationEx)
  KV_MEMBER_RENAME(spendingBlock, spending_block)
  KV_MEMBER_RENAME(spendingTransactionHash, spending_transaction_hash)
  KV_MEMBER_RENAME(inputInTransaction, input_in_transaction)
  KV_END_SERIALIZATION
};

enum class KeyImageState { Unconfirmed, Confirmed, Spent };

struct KeyOutputInfo {
  KeyImageState state;
  size_t count;
};

class TransfersContainer : public ITransfersContainer {
 public:
  TransfersContainer(const CryptoNote::Currency& currency, Logging::ILogger& logger, size_t transactionSpendableAge);

  bool addTransaction(const TransactionBlockInfo& block, const ITransactionReader& tx,
                      const std::vector<TransactionOutputInformationIn>& transfers);
  bool deleteUnconfirmedTransaction(const Crypto::Hash& transactionHash);
  bool markTransactionConfirmed(const TransactionBlockInfo& block, const Crypto::Hash& transactionHash,
                                const std::vector<uint32_t>& globalIndices);

  std::vector<Crypto::Hash> detach(BlockHeight height);
  bool advanceHeight(BlockHeight height);

  // ITransfersContainer
  virtual size_t transfersCount() const override;
  virtual size_t transactionsCount() const override;
  virtual uint64_t balance(uint32_t flags) const override;
  virtual void getOutputs(std::vector<TransactionOutputInformation>& transfers, uint32_t flags) const override;
  virtual bool getTransactionInformation(const Crypto::Hash& transactionHash, TransactionInformation& info,
                                         uint64_t* amountIn = nullptr, uint64_t* amountOut = nullptr) const override;
  virtual std::vector<TransactionOutputInformation> getTransactionOutputs(const Crypto::Hash& transactionHash,
                                                                          uint32_t flags) const override;
  // only type flags are feasible for this function
  virtual std::vector<TransactionOutputInformation> getTransactionInputs(const Crypto::Hash& transactionHash,
                                                                         uint32_t flags) const override;
  virtual void getUnconfirmedTransactions(std::vector<Crypto::Hash>& transactions) const override;
  virtual std::vector<TransactionSpentOutputInformation> getSpentOutputs() const override;

  // IStreamSerializable
  [[nodiscard]] virtual bool save(std::ostream& os) override;
  [[nodiscard]] virtual bool load(std::istream& in) override;

 private:
  struct ContainingTransactionIndex {};
  struct SpendingTransactionIndex {};
  struct SpentOutputDescriptorIndex {};

  typedef boost::multi_index_container<
      TransactionInformation,
      boost::multi_index::indexed_by<boost::multi_index::hashed_unique<BOOST_MULTI_INDEX_MEMBER(
                                         TransactionInformation, Crypto::Hash, transactionHash)>,
                                     boost::multi_index::ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(
                                         TransactionInformation, BlockHeight, blockHeight)> > >
      TransactionMultiIndex;

  typedef boost::multi_index_container<
      TransactionOutputInformationEx,
      boost::multi_index::indexed_by<
          boost::multi_index::hashed_non_unique<
              boost::multi_index::tag<SpentOutputDescriptorIndex>,
              boost::multi_index::const_mem_fun<TransactionOutputInformationEx, SpentOutputDescriptor,
                                                &TransactionOutputInformationEx::getSpentOutputDescriptor>,
              SpentOutputDescriptorHasher>,
          boost::multi_index::hashed_non_unique<
              boost::multi_index::tag<ContainingTransactionIndex>,
              boost::multi_index::const_mem_fun<TransactionOutputInformationEx, const Crypto::Hash&,
                                                &TransactionOutputInformationEx::getTransactionHash> > > >
      UnconfirmedTransfersMultiIndex;

  typedef boost::multi_index_container<
      TransactionOutputInformationEx,
      boost::multi_index::indexed_by<
          boost::multi_index::hashed_non_unique<
              boost::multi_index::tag<SpentOutputDescriptorIndex>,
              boost::multi_index::const_mem_fun<TransactionOutputInformationEx, SpentOutputDescriptor,
                                                &TransactionOutputInformationEx::getSpentOutputDescriptor>,
              SpentOutputDescriptorHasher>,
          boost::multi_index::hashed_non_unique<
              boost::multi_index::tag<ContainingTransactionIndex>,
              boost::multi_index::const_mem_fun<TransactionOutputInformationEx, const Crypto::Hash&,
                                                &TransactionOutputInformationEx::getTransactionHash> > > >
      AvailableTransfersMultiIndex;

  typedef boost::multi_index_container<
      SpentTransactionOutput,
      boost::multi_index::indexed_by<
          boost::multi_index::hashed_unique<
              boost::multi_index::tag<SpentOutputDescriptorIndex>,
              boost::multi_index::const_mem_fun<TransactionOutputInformationEx, SpentOutputDescriptor,
                                                &TransactionOutputInformationEx::getSpentOutputDescriptor>,
              SpentOutputDescriptorHasher>,
          boost::multi_index::hashed_non_unique<
              boost::multi_index::tag<ContainingTransactionIndex>,
              boost::multi_index::const_mem_fun<TransactionOutputInformationEx, const Crypto::Hash&,
                                                &SpentTransactionOutput::getTransactionHash> >,
          boost::multi_index::hashed_non_unique<
              boost::multi_index::tag<SpendingTransactionIndex>,
              boost::multi_index::const_mem_fun<SpentTransactionOutput, const Crypto::Hash&,
                                                &SpentTransactionOutput::getSpendingTransactionHash> > > >
      SpentTransfersMultiIndex;

 private:
  void addTransaction(const TransactionBlockInfo& block, const ITransactionReader& tx);
  bool addTransactionOutputs(const TransactionBlockInfo& block, const ITransactionReader& tx,
                             const std::vector<TransactionOutputInformationIn>& transfers);
  bool addTransactionInputs(const TransactionBlockInfo& block, const ITransactionReader& tx);
  void deleteTransactionTransfers(const Crypto::Hash& transactionHash);
  bool isSpendTimeUnlocked(uint64_t unlockTime) const;
  bool isIncluded(const TransactionOutputInformationEx& info, uint32_t flags) const;
  static bool isIncluded(TransactionTypes::OutputType type, uint32_t state, uint32_t flags);
  void updateTransfersVisibility(const Crypto::KeyImage& keyImage);

  void copyToSpent(const TransactionBlockInfo& block, const ITransactionReader& tx, size_t inputIndex,
                   const TransactionOutputInformationEx& output);
  void repair();

 private:
  TransactionMultiIndex m_transactions;
  UnconfirmedTransfersMultiIndex m_unconfirmedTransfers;
  AvailableTransfersMultiIndex m_availableTransfers;
  SpentTransfersMultiIndex m_spentTransfers;

  BlockHeight m_currentHeight;  // current height is needed to check if a transfer is unlocked
  size_t m_transactionSpendableAge;
  const CryptoNote::Currency& m_currency;
  mutable std::mutex m_mutex;
  Logging::LoggerRef m_logger;
};

}  // namespace CryptoNote
