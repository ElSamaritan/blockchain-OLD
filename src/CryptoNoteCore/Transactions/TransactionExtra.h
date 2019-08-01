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

#include <vector>
#include <string>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <CryptoNoteCore/CryptoNote.h>

namespace CryptoNote {

void setTransactionPublicKeyToExtra(TransactionExtra& tx_extra, const Crypto::PublicKey& key);
Crypto::PublicKey getTransactionPublicKeyFromExtra(const TransactionExtra& tx_extra);

bool addTransactionPublicKeyToExtra(TransactionExtra& tx_extra, const Crypto::PublicKey& tx_pub_key);
void setPaymentIdToTransactionExtraNonce(TransactionExtra& tx_extra, const PaymentId& payment_id);
bool getPaymentIdFromTransactionExtraNonce(const TransactionExtra& tx_extra, PaymentId& payment_id);

bool createTxExtraWithPaymentId(const std::string& paymentIdString, TransactionExtra& extra);
// returns false if payment id is not found or parse error
bool getPaymentIdFromTxExtra(const TransactionExtra& extra, PaymentId& paymentId);
bool parsePaymentId(const std::string& paymentIdString, PaymentId& paymentId);

}  // namespace CryptoNote
