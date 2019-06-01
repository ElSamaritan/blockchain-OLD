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

#pragma once

#include <cinttypes>
#include <string>

#include <Serialization/ISerializer.h>
#include <Xi/Types/Integral.hpp>

namespace Xi {
namespace Blockchain {
namespace Block {

/*!
 *
 * \brief The Version struct encapsulates an unsigned integer representing the consensus configuration of a given block.
 *
 * To change consensus in the blockchain, the block version has to be changed. Every validation of the consensus, that
 * may change, has to be dependent on the block version, data is embedded in.
 *
 * That way the blockchain can be updated/downgraded either by enforcing a change for given height or voting for a
 * change (\see Voting System).
 *
 */
struct Version : enable_integral_from_this<uint8_t, Version> {
  using enable_integral_from_this::enable_integral_from_this;

  /*!
   * \brief Null Well defined optional state, the null value is interpreted as no value given.
   */
  static const Version Null;

  /*!
   * \brief Genesis The minimum block version, every blockchain should start using this version.
   */
  static const Version Genesis;

  /*!
   * \brief isNull checks if this version corresponds to no version given.
   * \return (*this) == Version::Null
   */
  bool isNull() const;

 private:
  friend bool serialize(Version&, Common::StringView, CryptoNote::ISerializer&);
  friend std::string toString(const Version);
};

std::string toString(const Version version);

[[nodiscard]] bool serialize(Version& version, Common::StringView name, CryptoNote::ISerializer& serializer);

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi
