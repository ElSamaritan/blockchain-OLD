/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include "Xi/Crypto/PasswordContainer.h"

#include <algorithm>
#include <limits>

#include <Xi/Utils/ExternalIncludePush.h>
#include <openssl/evp.h>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

Xi::Crypto::PasswordContainer::PasswordContainer(const std::string &password) {
  generateConfig();
  m_hash = computeHash(password);
}

bool Xi::Crypto::PasswordContainer::validate(const std::string &password) const {
  return hashEqual(m_hash, computeHash(password));
}

Xi::Crypto::PasswordContainer::hash_t Xi::Crypto::PasswordContainer::computeHash(const std::string &password) const {
  hash_t hash;
  PKCS5_PBKDF2_HMAC(password.data(), static_cast<int>(password.size()),
                    reinterpret_cast<const uint8_t *>(m_salt.data()), static_cast<int>(m_salt.size()),
                    static_cast<int>(m_iterations), EVP_sha512(), static_cast<int>(hash.size()), hash.data());
  return hash;
}

void Xi::Crypto::PasswordContainer::generateConfig() {
  boost::random::random_device rng;
  rng.generate(m_salt.begin(), m_salt.end());
  boost::random::uniform_int_distribution<uint32_t> dist{std::numeric_limits<uint16_t>::max(),
                                                         2u * std::numeric_limits<uint16_t>::max()};
  m_iterations = dist(rng);
}

bool Xi::Crypto::PasswordContainer::hashEqual(const PasswordContainer::hash_t &lhs,
                                              const PasswordContainer::hash_t &rhs) {
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
