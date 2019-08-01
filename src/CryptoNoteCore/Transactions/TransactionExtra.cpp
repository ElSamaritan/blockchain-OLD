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

#include "CryptoNoteCore/Transactions/TransactionExtra.h"

#include <Xi/Global.hh>
#include <Xi/Exceptions.hpp>

#include <Common/MemoryInputStream.h>
#include <Common/StreamTools.h>
#include <Common/StringTools.h>
#include <Serialization/BinaryOutputStreamSerializer.h>
#include <Serialization/BinaryInputStreamSerializer.h>

#include "CryptoNoteCore/CryptoNoteTools.h"

using namespace Crypto;
using namespace Common;

namespace CryptoNote {

PublicKey getTransactionPublicKeyFromExtra(const TransactionExtra& tx_extra) {
  return tx_extra.publicKey.value_or(Crypto::PublicKey::Null);
}

bool addTransactionPublicKeyToExtra(TransactionExtra& tx_extra, const PublicKey& tx_pub_key) {
  tx_extra.features |= TransactionExtraFeature::PublicKey;
  tx_extra.publicKey = tx_pub_key;
  return true;
}

void setPaymentIdToTransactionExtraNonce(TransactionExtra& tx_extra, const PaymentId& payment_id) {
  tx_extra.features |= TransactionExtraFeature::PaymentId;
  tx_extra.paymentId = payment_id;
}

bool getPaymentIdFromTransactionExtraNonce(TransactionExtra& tx_extra, PaymentId& payment_id) {
  XI_RETURN_SC_IF_NOT(tx_extra.paymentId, false);
  payment_id = *tx_extra.paymentId;
  XI_RETURN_SC(true);
}

bool parsePaymentId(const std::string& paymentIdString, PaymentId& paymentId) {
  if (auto pid = PaymentId::fromString(paymentIdString); pid.isError()) {
    return false;
  } else {
    if (!pid->isValid()) {
      return false;
    } else {
      paymentId = pid.take();
      return true;
    }
  }
}

bool createTxExtraWithPaymentId(const std::string& paymentIdString, TransactionExtra& extra) {
  PaymentId paymentIdBin{};
  if (!parsePaymentId(paymentIdString, paymentIdBin)) {
    return false;
  }
  setPaymentIdToTransactionExtraNonce(extra, paymentIdBin);
  return true;
}

bool getPaymentIdFromTxExtra(const TransactionExtra& extra, PaymentId& paymentId) {
  XI_RETURN_SC_IF_NOT(extra.paymentId.has_value(), false);
  paymentId = *extra.paymentId;
  XI_RETURN_SC(true);
}

void setTransactionPublicKeyToExtra(TransactionExtra& tx_extra, const PublicKey& key) {
  using namespace Xi;
  exceptional_if_not<InvalidArgumentError>(key.isValid(), "invalid public key for transaction extra: {}",
                                           key.toString());
  tx_extra.features |= TransactionExtraFeature::PublicKey;
  tx_extra.publicKey = key;
}

bool getPaymentIdFromTransactionExtraNonce(const TransactionExtra& tx_extra, PaymentId& payment_id) {
  XI_RETURN_SC_IF_NOT(tx_extra.paymentId, false);
  payment_id = *tx_extra.paymentId;
  XI_RETURN_SC(true);
}

}  // namespace CryptoNote
