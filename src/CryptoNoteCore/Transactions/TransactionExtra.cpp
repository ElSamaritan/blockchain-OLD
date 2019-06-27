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

#include "CryptoNoteCore/Transactions/TransactionExtra.h"

#include <Xi/Global.hh>

#include <Common/MemoryInputStream.h>
#include <Common/StreamTools.h>
#include <Common/StringTools.h>
#include <Serialization/BinaryOutputStreamSerializer.h>
#include <Serialization/BinaryInputStreamSerializer.h>

#include "CryptoNoteCore/CryptoNoteTools.h"

using namespace Crypto;
using namespace Common;

namespace CryptoNote {

bool parseTransactionExtra(const std::vector<uint8_t>& transactionExtra,
                           std::vector<TransactionExtraField>& transactionExtraFields) {
  transactionExtraFields.clear();
  if (transactionExtra.empty()) return true;

  try {
    MemoryInputStream iss(transactionExtra.data(), transactionExtra.size());
    BinaryInputStreamSerializer ar(iss);

    int c = 0;

    while (!iss.endOfStream()) {
      c = read<uint8_t>(iss);

      if (c == TX_EXTRA_TAG_PADDING) {
        TransactionExtraPadding pad{};
        XI_RETURN_EC_IF_NOT(ar(pad.size, "padding_size"), false);
        XI_RETURN_EC_IF(pad.size > TX_EXTRA_PADDING_MAX_COUNT, false);
        std::vector<uint8_t> paddedBytes;
        read(iss, paddedBytes, pad.size);
        XI_RETURN_EC_IF_NOT(paddedBytes.size() == pad.size, false);
        for (size_t i = 0; i < paddedBytes.size(); ++i) {
          XI_RETURN_EC_IF_NOT(paddedBytes[i] == 0u, false);
        }
        transactionExtraFields.emplace_back(pad);
      } else if (c == TX_EXTRA_TAG_PUBKEY) {
        TransactionExtraPublicKey pub{};
        XI_RETURN_EC_IF_NOT(ar(pub.publicKey, "public_key"), false);
        transactionExtraFields.emplace_back(pub);
      } else if (c == TX_EXTRA_TAG_NONCE) {
        TransactionExtraNonce nonce{};
        XI_RETURN_EC_IF_NOT(ar(nonce.nonce, "nonce"), false);
        XI_RETURN_EC_IF(nonce.nonce.size() > TX_EXTRA_NONCE_MAX_COUNT, false);
        transactionExtraFields.emplace_back(nonce);
      } else {
        return false;
      }
    }
  } catch (std::exception&) {
    return false;
  }

  return true;
}

struct ExtraSerializerVisitor : public boost::static_visitor<bool> {
  std::vector<uint8_t>& extra;

  ExtraSerializerVisitor(std::vector<uint8_t>& tx_extra) : extra(tx_extra) {}

  bool operator()(const TransactionExtraPadding& t) {
    if (t.size > TX_EXTRA_PADDING_MAX_COUNT) {
      return false;
    }
    extra.insert(extra.end(), t.size, 0);
    return true;
  }

  bool operator()(const TransactionExtraPublicKey& t) { return addTransactionPublicKeyToExtra(extra, t.publicKey); }

  bool operator()(const TransactionExtraNonce& t) { return addExtraNonceToTransactionExtra(extra, t.nonce); }
};

bool writeTransactionExtra(std::vector<uint8_t>& tx_extra, const std::vector<TransactionExtraField>& tx_extra_fields) {
  ExtraSerializerVisitor visitor(tx_extra);

  for (const auto& tag : tx_extra_fields) {
    if (!boost::apply_visitor(visitor, tag)) {
      return false;
    }
  }

  return true;
}

PublicKey getTransactionPublicKeyFromExtra(const std::vector<uint8_t>& tx_extra) {
  std::vector<TransactionExtraField> tx_extra_fields;
  if (!parseTransactionExtra(tx_extra, tx_extra_fields)) return PublicKey::Null;

  TransactionExtraPublicKey pub_key_field;
  if (!findTransactionExtraFieldByType(tx_extra_fields, pub_key_field)) return PublicKey::Null;

  return pub_key_field.publicKey;
}

bool addTransactionPublicKeyToExtra(std::vector<uint8_t>& tx_extra, const PublicKey& tx_pub_key) {
  tx_extra.resize(tx_extra.size() + 1 + tx_pub_key.size());
  tx_extra[tx_extra.size() - 1 - tx_pub_key.size()] = TX_EXTRA_TAG_PUBKEY;
  std::memcpy(&tx_extra[tx_extra.size() - sizeof(PublicKey)], tx_pub_key.data(), tx_pub_key.size());
  return true;
}

bool addExtraNonceToTransactionExtra(std::vector<uint8_t>& tx_extra, const BinaryArray& extra_nonce) {
  if (extra_nonce.size() > TX_EXTRA_NONCE_MAX_COUNT) {
    return false;
  }

  size_t start_pos = tx_extra.size();
  tx_extra.resize(tx_extra.size() + 2 + extra_nonce.size());
  // write tag
  tx_extra[start_pos] = TX_EXTRA_TAG_NONCE;
  // write len
  ++start_pos;
  tx_extra[start_pos] = static_cast<uint8_t>(extra_nonce.size());
  // write data
  ++start_pos;
  memcpy(&tx_extra[start_pos], extra_nonce.data(), extra_nonce.size());
  return true;
}

void setPaymentIdToTransactionExtraNonce(std::vector<uint8_t>& extra_nonce, const PaymentId& payment_id) {
  extra_nonce.clear();
  extra_nonce.push_back(TX_EXTRA_NONCE_PAYMENT_ID);
  const uint8_t* payment_id_ptr = payment_id.data();
  std::copy(payment_id_ptr, payment_id_ptr + payment_id.size(), std::back_inserter(extra_nonce));
}

bool getPaymentIdFromTransactionExtraNonce(const std::vector<uint8_t>& extra_nonce, PaymentId& payment_id) {
  if (PublicKey::bytes() + 1 != extra_nonce.size()) return false;
  if (TX_EXTRA_NONCE_PAYMENT_ID != extra_nonce[0]) return false;
  payment_id = PaymentId::Null;
  std::memcpy(payment_id.data(), extra_nonce.data() + 1, PublicKey::bytes());
  if (!payment_id.isValid()) {
    payment_id.nullify();
    return false;
  } else {
    return true;
  }
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

bool createTxExtraWithPaymentId(const std::string& paymentIdString, std::vector<uint8_t>& extra) {
  PaymentId paymentIdBin;

  if (!parsePaymentId(paymentIdString, paymentIdBin)) {
    return false;
  }

  std::vector<uint8_t> extraNonce;
  CryptoNote::setPaymentIdToTransactionExtraNonce(extraNonce, paymentIdBin);

  if (!CryptoNote::addExtraNonceToTransactionExtra(extra, extraNonce)) {
    return false;
  }

  return true;
}

bool getPaymentIdFromTxExtra(const std::vector<uint8_t>& extra, PaymentId& paymentId) {
  std::vector<TransactionExtraField> tx_extra_fields;
  if (!parseTransactionExtra(extra, tx_extra_fields)) {
    return false;
  }

  TransactionExtraNonce extra_nonce;
  if (findTransactionExtraFieldByType(tx_extra_fields, extra_nonce)) {
    if (!getPaymentIdFromTransactionExtraNonce(extra_nonce.nonce, paymentId)) {
      return false;
    }
  } else {
    return false;
  }

  return true;
}

}  // namespace CryptoNote
