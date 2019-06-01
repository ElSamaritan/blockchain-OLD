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

#include "Xi/Blockchain/Block/Version.hpp"

const Xi::Blockchain::Block::Version Xi::Blockchain::Block::Version::Null{0};
const Xi::Blockchain::Block::Version Xi::Blockchain::Block::Version::Genesis{1};

bool Xi::Blockchain::Block::serialize(Xi::Blockchain::Block::Version &version, Common::StringView name,
                                      CryptoNote::ISerializer &serializer) {
  return serializer(version.value, name);
}

std::string Xi::Blockchain::Block::toString(const Xi::Blockchain::Block::Version version) {
  return std::string{"v"} + std::to_string(version.value);
}

bool Xi::Blockchain::Block::Version::isNull() const { return *this == Null; }
