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

#include "CryptoNoteCore/HashAlgorithms.h"

#include <Xi/Crypto/Hash/Sha2.hh>
#include <Xi/Crypto/Hash/Keccak.hh>

#include "crypto/cnx/cnx.h"
#include "crypto/hash-predef.h"

// TODO Error return codes.
void CryptoNote::Hashes::CNX_v1::operator()(Xi::ConstByteSpan blob, Crypto::Hash &hash) const {
  Crypto::CNX::Hash_v1{}(blob.data(), blob.size(), hash);
}

void CryptoNote::Hashes::Sha2_256::operator()(Xi::ConstByteSpan blob, Crypto::Hash &hash) const {
  xi_crypto_hash_sha2_256(blob.data(), blob.size_bytes(), hash.data());
}

void CryptoNote::Hashes::Keccak::operator()(Xi::ConstByteSpan blob, Crypto::Hash &hash) const {
  xi_crypto_hash_keccak_256(blob.data(), blob.size_bytes(), hash.data());
}

std::shared_ptr<CryptoNote::Hashes::IProofOfWorkAlgorithm> CryptoNote::Hashes::makeProofOfWorkAlgorithm(
    std::string_view id) {
  if (id == "CNX-v1") {
    return std::make_shared<CNX_v1>();
  } else if (id == "SHA2-256") {
    return std::make_shared<Sha2_256>();
  } else if (id == "Keccak") {
    return std::make_shared<Keccak>();
  } else {
    return nullptr;
  }
}

std::vector<std::string> CryptoNote::Hashes::supportedProofOfWorkAlgorithms() {
  return {{
      "CNX-v1",
      "SHA2-256",
      "Keccak",
  }};
}
