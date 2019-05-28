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

#include "WalletLegacySerialization.h"
#include "WalletLegacy/WalletUnconfirmedTransactions.h"
#include "IWalletLegacy.h"

#include "CryptoNoteCore/CryptoNoteSerialization.h"
#include "Serialization/ISerializer.h"
#include "Serialization/SerializationOverloads.h"

namespace CryptoNote {

bool serialize(UnconfirmedTransferDetails& utd, CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(utd.tx, "transaction"), false);
  XI_RETURN_EC_IF_NOT(serializer(utd.amount, "amount"), false);
  XI_RETURN_EC_IF_NOT(serializer(utd.outsAmount, "outs_amount"), false);
  uint64_t time = static_cast<uint64_t>(utd.sentTime);
  XI_RETURN_EC_IF_NOT(serializer(time, "sent_time"), false);
  utd.sentTime = static_cast<time_t>(time);
  uint64_t txId = static_cast<uint64_t>(utd.transactionId);
  XI_RETURN_EC_IF_NOT(serializer(txId, "transaction_id"), false);
  utd.transactionId = static_cast<size_t>(txId);
  return true;
}

bool serialize(WalletLegacyTransaction& txi, CryptoNote::ISerializer& serializer) {
  uint64_t trId = static_cast<uint64_t>(txi.firstTransferId);
  XI_RETURN_EC_IF_NOT(serializer(trId, "first_transfer_id"), false);
  txi.firstTransferId = static_cast<size_t>(trId);

  uint64_t trCount = static_cast<uint64_t>(txi.transferCount);
  XI_RETURN_EC_IF_NOT(serializer(trCount, "transfer_count"), false);
  txi.transferCount = static_cast<size_t>(trCount);

  XI_RETURN_EC_IF_NOT(serializer(txi.totalAmount, "total_amount"), false);

  XI_RETURN_EC_IF_NOT(serializer(txi.fee, "fee"), false);
  XI_RETURN_EC_IF_NOT(serializer(txi.hash, "hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(txi.isCoinbase, "is_coinbase"), false);

  CryptoNote::serializeBlockHeight(serializer, txi.blockHeight, "block_height");

  XI_RETURN_EC_IF_NOT(serializer(txi.timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializer(txi.unlockTime, "unlock_time"), false);
  XI_RETURN_EC_IF_NOT(serializer(txi.extra, "extra"), false);

  // this field has been added later in the structure.
  // in order to not break backward binary compatibility
  // we just set it to zero
  txi.sentTime = 0;
  return true;
}

bool serialize(WalletLegacyTransfer& tr, CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(tr.address, "address"), false);
  XI_RETURN_EC_IF_NOT(serializer(tr.amount, "amount"), false);
  return true;
}

}  // namespace CryptoNote
