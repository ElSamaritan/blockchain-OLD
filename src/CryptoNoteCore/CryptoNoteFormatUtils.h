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

#pragma once

#include <utility>
#include <string>
#include <vector>

#include <boost/utility/value_init.hpp>

#include "CryptoNoteBasic.h"
#include "CryptoNoteSerialization.h"
#include "CryptoNoteCore/Currency.h"

#include "Serialization/BinaryOutputStreamSerializer.h"
#include "Serialization/BinaryInputStreamSerializer.h"

namespace Logging {
class ILogger;
}

namespace CryptoNote {

struct TransactionSourceEntry {
  typedef std::pair<uint32_t, Crypto::PublicKey> OutputEntry;

  std::vector<OutputEntry> outputs;            // index + key
  size_t realOutput;                           // index in outputs vector of real output_entry
  Crypto::PublicKey realTransactionPublicKey;  // incoming real tx public key
  size_t realOutputIndexInTransaction;         // index in transaction outputs vector
  uint64_t amount;                             // money
};

struct TransactionDestinationEntry {
  uint64_t amount;            // money
  AccountPublicAddress addr;  // destination address

  TransactionDestinationEntry() : amount(0), addr(boost::value_initialized<AccountPublicAddress>()) {
  }
  TransactionDestinationEntry(uint64_t amount, const AccountPublicAddress& addr) : amount(amount), addr(addr) {
  }
};

bool constructTransaction(const AccountKeys& senderAccountKeys, const std::vector<TransactionSourceEntry>& sources,
                          const std::vector<TransactionDestinationEntry>& destinations, TransactionExtra extra,
                          Transaction& transaction, uint64_t unlock_time, Logging::ILogger& log,
                          const Currency& currency, const BlockVersion blockVersion);

bool is_out_to_acc(const AccountKeys& acc, const KeyOutput& out_key, const Crypto::PublicKey& tx_pub_key,
                   size_t keyIndex);
bool is_out_to_acc(const AccountKeys& acc, const KeyOutput& out_key, const Crypto::KeyDerivation& derivation,
                   size_t keyIndex);
bool lookup_acc_outs(const AccountKeys& acc, const Transaction& tx, const Crypto::PublicKey& tx_pub_key,
                     std::vector<size_t>& outs, uint64_t& money_transfered);
bool lookup_acc_outs(const AccountKeys& acc, const Transaction& tx, std::vector<size_t>& outs,
                     uint64_t& money_transfered);
bool get_tx_fee(const Transaction& tx, uint64_t& fee);
uint64_t get_tx_fee(const Transaction& tx);
bool generate_key_image_helper(const AccountKeys& ack, const Crypto::PublicKey& tx_public_key, size_t real_output_index,
                               KeyPair& in_ephemeral, Crypto::KeyImage& ki);
bool getInputsMoneyAmount(const Transaction& tx, uint64_t& money);
bool checkInputTypesSupported(const TransactionPrefix& tx);
bool checkOutsValid(const TransactionPrefix& tx, std::string* error = nullptr);
bool checkMoneyOverflow(const TransactionPrefix& tx);
bool checkInputsOverflow(const TransactionPrefix& tx);
bool checkOutsOverflow(const TransactionPrefix& tx);
uint64_t get_outs_money_amount(const Transaction& tx);
std::string short_hash_str(const Crypto::Hash& h);

// 62387000000 -> 455827 + 7000000 + 80000000 + 300000000 + 2000000000 + 60000000000
template <typename chunk_handler_t>
void decompose_amount_into_digits(uint64_t amount, const chunk_handler_t& chunk_handler) {
  if (0 == amount) {
    return;
  }

  uint64_t order = 1;
  while (0 != amount) {
    uint64_t chunk = (amount % 10) * order;
    amount /= 10;
    order *= 10;

    if (0 != chunk) {
      chunk_handler(chunk);
    }
  }
}

}  // namespace CryptoNote
