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

#include "Serialization/TypeTag.hpp"

#include <utility>
#include <limits>
#include <cinttypes>

const CryptoNote::TypeTag CryptoNote::TypeTag::Null{NoBinaryTag, NoTextTag};

const CryptoNote::TypeTag::binary_type CryptoNote::TypeTag::NoBinaryTag{
    std::numeric_limits<CryptoNote::TypeTag::binary_type>::min()};

const CryptoNote::TypeTag::text_type CryptoNote::TypeTag::NoTextTag{""};

CryptoNote::TypeTag::TypeTag(binary_type binary, text_type text) : m_binary{binary}, m_text{std::move(text)} {
}

CryptoNote::TypeTag::binary_type CryptoNote::TypeTag::binary() const {
  return m_binary;
}

const CryptoNote::TypeTag::text_type &CryptoNote::TypeTag::text() const {
  return m_text;
}

bool CryptoNote::TypeTag::isNull() const {
  return binary() == NoBinaryTag && text() == NoTextTag;
}

bool CryptoNote::TypeTag::operator==(const CryptoNote::TypeTag &rhs) const {
  if (binary() != NoBinaryTag && binary() == rhs.binary()) {
    return true;
  } else if (text() != NoTextTag && text() == rhs.text()) {
    return true;
  } else {
    return false;
  }
}

bool CryptoNote::TypeTag::operator!=(const CryptoNote::TypeTag &rhs) const {
  return !(*this == rhs);
}
