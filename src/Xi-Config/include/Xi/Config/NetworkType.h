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

#include <string>
#include <cinttypes>

#include <Xi/Algorithm/String.h>
#include <Serialization/EnumSerialization.hpp>

namespace Xi {
namespace Config {
namespace Network {
/*!
 * \brief The Type enum seperates multiple networks of Xi to not interfer with each other.
 *
 * In order to test upcoming changes and have an evaluation phase before rolling out new releases Xi distinguishes
 * multiple networks that are not compatible with each other.
 *
 * For more information what the purpose of every network is, please have a look at the specific enum entries.
 */
enum Type : uint8_t {
  MainNet = 1,   ///< This indicates the production network. Meaning you are on the official blockchain most users are
                 ///< connected to.
  StageNet = 2,  ///< This network is for testing new releases that are nearly ready for production.
  TestNet =
      3,  ///< A network that may shutdown every moment or is not even compatible to any other chain. This network is
          /// used internally by the developers to test new features. If you are not explicitly invited you shall not
          /// use it.
  LocalTestNet = 4,  ///< This network is used by developers to test new configurations locally. You are not able to
                     ///< reach it from any other machine.
};
XI_SERIALIZATION_ENUM(Type)

std::string toString(Type type);
}  // namespace Network
}  // namespace Config
std::string to_string(Xi::Config::Network::Type type);
template <>
::Xi::Config::Network::Type lexical_cast<::Xi::Config::Network::Type>(const std::string& str);
}  // namespace Xi

namespace CryptoNote {
using NetworkType = Xi::Config::Network::Type;
}

XI_SERIALIZATION_ENUM_RANGE(Xi::Config::Network::Type, MainNet, LocalTestNet)
XI_SERIALIZATION_ENUM_TAG(Xi::Config::Network::Type, MainNet, "main")
XI_SERIALIZATION_ENUM_TAG(Xi::Config::Network::Type, StageNet, "staging")
XI_SERIALIZATION_ENUM_TAG(Xi::Config::Network::Type, TestNet, "testing")
XI_SERIALIZATION_ENUM_TAG(Xi::Config::Network::Type, LocalTestNet, "local")
