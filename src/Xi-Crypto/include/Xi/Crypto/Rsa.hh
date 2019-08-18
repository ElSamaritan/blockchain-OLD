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

#pragma once

#include <Xi/Global.hh>
#include <Xi/Byte.hh>

#if defined(__cplusplus)
extern "C" {
#endif  // defined(__cplusplus)

#include <stdio.h>

/*!
 * \brief xi_crypto_rsa_sign Signs the sha2-512 hash of a message.
 * \param message The message to sign.
 * \param messageLength The length of the message to sign.
 * \param secretKey Pointer to secret key in PEM format.
 * \param secretKeyLength Binary size of the screteKey blob.
 * \param signature Output pointer for the signature.
 * \param signatureLength Output pointer for the signature length.
 * \return XI_RETURN_CODE_SUCCESS if the signature generation succeeded.
 *
 * \attention If this method succeeds the caller is responsable to free the signature.
 * \todo Support password interface.
 */
int xi_crypto_rsa_sign(const xi_byte_t* message, const size_t messageLength, const xi_byte_t* secretKey,
                       const size_t secretKeySize, xi_byte_t** signature, size_t* signatureLength);

/*!
 * \brief xi_crypto_rsa_verify Verifies a signature against the sha2-512 hash of a message.
 * \param message The message to verify.
 * \param messageLength The length of the message to verify.
 * \param publicKey The expected signers public key in PEM format.
 * \param publicKeyLength The epected signes public key blob size.
 * \param signature The provided signature blob.
 * \param signatureLength The provided signature blob size.
 * \return  XI_RETURN_CODE_SUCCESS iff the signature is valid for the given public key and message.
 */
int xi_crypto_rsa_verify(const xi_byte_t* message, const size_t messageLength, const xi_byte_t* publicKey,
                         const size_t publicKeyLength, const xi_byte_t* signature, const size_t signatureLength);

#if defined(__cplusplus)
}
#endif  // defined(__cplusplus)

#if defined(__cplusplus)
#include <string>

#include <Xi/Result.h>

namespace Xi {
namespace Crypto {
namespace Rsa {

/*!
 * \brief sign Creates a binary signature of a message using a given RSA (PEM) secret key.
 * \param message The message to sign.
 * \param secretKey The secret key to use as signer.
 * \return An error on failure otherwise the signature blob.
 */
Result<ByteVector> sign(ConstByteSpan message, ConstByteSpan secretKey);

[[nodiscard]] bool verify(ConstByteSpan message, ConstByteSpan publicKey, ConstByteSpan signature);

}  // namespace Rsa
}  // namespace Crypto
}  // namespace Xi
#endif  // defined(__cplusplus)
