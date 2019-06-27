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

#include "Xi/Crypto/Chacha8.h"

#include <limits>

#include <Xi/ExternalIncludePush.h>
#include <openssl/evp.h>
#include <Xi/ExternalIncludePop.h>

void Xi::Crypto::Chacha8::generate_key(const std::string &password, uint8_t *data, size_t len) {
  PKCS5_PBKDF2_HMAC(password.data(), static_cast<int>(password.size()), NULL, 0, std::numeric_limits<uint16_t>::max(),
                    EVP_sha512(), static_cast<int>(len), data);
}
