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

#include "CryptoNoteCore/Transactions/Validation.h"

#include <algorithm>

#include <Xi/Exceptional.h>

using Error = CryptoNote::error::TransactionValidationError;

Error CryptoNote::validateExtra(const CryptoNote::Transaction &tx) {
  XI_RETURN_EC_IF_NOT(validateExtraSize(tx), Error::EXTRA_TOO_LARGE);
  std::vector<TransactionExtraField> fields;
  XI_RETURN_EC_IF_NOT(parseTransactionExtra(tx.extra, fields), Error::EXTRA_ILL_FORMED);
  XI_RETURN_EC_IF_NOT(validateExtraPublicKeys(fields), Error::EXTRA_INVALID_PUBLIC_KEY);
  XI_RETURN_EC_IF_NOT(validateExtraNonce(fields), Error::EXTRA_INVALID_PUBLIC_KEY);
  return Error::VALIDATION_SUCCESS;
}

bool CryptoNote::validateExtraSize(const CryptoNote::Transaction &tx) { return tx.extra.size() <= TX_EXTRA_MAX_SIZE; }

bool CryptoNote::validateExtraPublicKeys(const std::vector<TransactionExtraField> &fields) {
  const auto isPublicKey = [](const auto &field) { return field.type() == typeid(TransactionExtraPublicKey); };
  auto search = std::find_if(fields.begin(), fields.end(), isPublicKey);
  XI_RETURN_EC_IF(search == fields.end(), false);
  const auto &pkField = boost::get<TransactionExtraPublicKey>(*search);
  XI_RETURN_EC_IF_NOT(validateExtraPublicKeys(pkField), false);
  auto another = std::find_if(search, fields.end(), isPublicKey);
  return another == fields.end();
}

bool CryptoNote::validateExtraPublicKeys(const CryptoNote::TransactionExtraPublicKey &pk) {
  return pk.publicKey.isValid();
}

bool CryptoNote::validateExtraCumulativePadding(const std::vector<CryptoNote::TransactionExtraField> &fields) {
  size_t padSize = 0;
  for (const auto &field : fields) {
    if (field.type() == typeid(TransactionExtraPadding)) {
      padSize += boost::get<TransactionExtraPadding>(field).size;
      XI_RETURN_EC_IF(padSize > TX_EXTRA_PADDING_MAX_COUNT, false);
    }
  }
  return true;
}

bool CryptoNote::validateExtraNonce(const std::vector<TransactionExtraField> &fields) {
  const auto isExtraNonce = [](const auto &field) { return field.type() == typeid(TransactionExtraNonce); };
  auto search = std::find_if(fields.begin(), fields.end(), isExtraNonce);
  XI_RETURN_EC_IF(search == fields.end(), false);
  const auto &nonceField = boost::get<TransactionExtraNonce>(*search);
  XI_RETURN_EC_IF_NOT(validateExtraNonce(nonceField), false);
  auto another = std::find_if(search, fields.end(), isExtraNonce);
  return another == fields.end();
}

bool CryptoNote::validateExtraNonce(const CryptoNote::TransactionExtraNonce &nonce) {
  XI_RETURN_EC_IF(nonce.nonce.empty(), false);
  XI_RETURN_EC_IF(nonce.nonce.size() > TX_EXTRA_NONCE_MAX_COUNT, false);

  if (nonce.nonce[0] == TX_EXTRA_NONCE_PAYMENT_ID) {
    XI_RETURN_EC_IF_NOT(nonce.nonce.size() == 1 + Crypto::Hash::bytes(), false);
    XI_RETURN_EC_IF(std::memcmp(&nonce.nonce[1], Crypto::Hash::Null.data(), Crypto::Hash::bytes()) == 0, false);
  } else {
    XI_RETURN_EC_IF_NOT(nonce.nonce[0] == TX_EXTRA_NONCE_CUSTOM_ID, false);
  }

  return true;
}
