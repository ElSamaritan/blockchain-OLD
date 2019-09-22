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

#include "Xi/Crypto/Rsa.hh"

#include <cstring>
#include <utility>
#include <cstdlib>

#include <Xi/Exceptions.hpp>

namespace Xi {
namespace Crypto {
namespace Rsa {

Result<ByteVector> sign(ConstByteSpan message, ConstByteSpan secretKey) {
  XI_ERROR_TRY
  xi_byte_t* signature = nullptr;
  size_t signatureLength = 0;
  const auto rc = xi_crypto_rsa_sign(message.data(), message.size_bytes(), secretKey.data(), secretKey.size_bytes(),
                                     &signature, &signatureLength);
  exceptional_if_not<RuntimeError>(rc == XI_RETURN_CODE_SUCCESS, "Rsa sign failed.");

  ByteVector reval{};
  reval.resize(signatureLength);
  std::memcpy(reval.data(), signature, signatureLength);
  free(signature);

  return success(std::move(reval));
  XI_ERROR_CATCH
}

bool verify(ConstByteSpan message, ConstByteSpan publicKey, ConstByteSpan signature) {
  const auto rc = xi_crypto_rsa_verify(message.data(), message.size_bytes(), publicKey.data(), publicKey.size_bytes(),
                                       signature.data(), signature.size_bytes());
  return rc == XI_RETURN_CODE_SUCCESS;
}

}  // namespace Rsa
}  // namespace Crypto
}  // namespace Xi
