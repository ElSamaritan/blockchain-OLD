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

#pragma once

#include <vector>
#include <array>

#include <Xi/Global.h>

namespace Xi {
namespace Crypto {
/*!
 * \brief The PasswordContainer class implements a self altering password validator without storing the password in
 * plain text.
 *
 * The container uses a cryptographic hash functions to store the hash of a password. Once the user tries to validate
 * his credentials his provided password is hashed again and compared against the stored hash of this container.
 *
 * Further the container salts the password to protect against magic/rainbow tables. Once a password has been validated
 * the container computes a new random salt and renews the hash.
 *
 * \attention Always store a user provided password as short as possible. You should immediately store the password in a
 * container or validate it against a container. After clear the password instantly.
 */
class PasswordContainer {
  using hash_t = std::array<uint8_t, 64>;
  using salt_t = std::array<int32_t, 64 / 8>;

 public:
  /*!
   * \brief PasswordContainer stores the password hash for later validation.
   * \param password The valid password to check user requests against.
   */
  explicit PasswordContainer(const std::string& password);

  XI_DEFAULT_COPY(PasswordContainer);
  XI_DEFAULT_MOVE(PasswordContainer);
  ~PasswordContainer() = default;

  /*!
   * \brief operator () validates a given user password against the expected password
   * \param password The user provided password to check.
   * \return True if access should be granted otherwise false
   */
  bool operator()(const std::string& password);

  /*!
   * \brief hash the stored password hash
   */
  const hash_t& hash() const;

 private:
  hash_t computeHash(const std::string& password) const;
  void generateConfig();

  static bool hashEqual(const hash_t& lhs, const hash_t& rhs);

 private:
  hash_t m_hash;
  salt_t m_salt;
  uint32_t m_iterations;
};
}  // namespace Crypto
}  // namespace Xi
