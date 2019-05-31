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

#include "CryptoNoteCore/Blockchain/BlockOffset.hpp"

CryptoNote::BlockOffset::BlockOffset(CryptoNote::BlockOffset::value_type value) : m_native{value} {}

CryptoNote::BlockOffset CryptoNote::BlockOffset::fromNative(const CryptoNote::BlockOffset::value_type value) {
  return BlockOffset{value};
}

CryptoNote::BlockOffset::BlockOffset() : m_native{0} {}

int64_t CryptoNote::BlockOffset::native() const { return m_native; }

bool CryptoNote::BlockOffset::operator==(const CryptoNote::BlockOffset rhs) const { return m_native == rhs.m_native; }
bool CryptoNote::BlockOffset::operator!=(const CryptoNote::BlockOffset rhs) const { return m_native != rhs.m_native; }
bool CryptoNote::BlockOffset::operator<(const CryptoNote::BlockOffset rhs) const { return m_native < rhs.m_native; }
bool CryptoNote::BlockOffset::operator<=(const CryptoNote::BlockOffset rhs) const { return m_native <= rhs.m_native; }
bool CryptoNote::BlockOffset::operator>(const CryptoNote::BlockOffset rhs) const { return m_native > rhs.m_native; }
bool CryptoNote::BlockOffset::operator>=(const CryptoNote::BlockOffset rhs) const { return m_native >= rhs.m_native; }

size_t std::hash<CryptoNote::BlockOffset>::operator()(const CryptoNote::BlockOffset offset) const {
  return std::hash<CryptoNote::BlockOffset::value_type>{}(offset.native());
}

CryptoNote::BlockOffset CryptoNote::operator+(const CryptoNote::BlockOffset lhs, const CryptoNote::BlockOffset rhs) {
  return BlockOffset::fromNative(lhs.native() + rhs.native());
}

CryptoNote::BlockOffset& CryptoNote::operator+=(CryptoNote::BlockOffset& lhs, const CryptoNote::BlockOffset rhs) {
  return lhs = BlockOffset::fromNative(lhs.native() + rhs.native());
}

CryptoNote::BlockOffset CryptoNote::operator-(const CryptoNote::BlockOffset lhs, const CryptoNote::BlockOffset rhs) {
  return BlockOffset::fromNative(lhs.native() - rhs.native());
}

CryptoNote::BlockOffset& CryptoNote::operator-=(CryptoNote::BlockOffset& lhs, const CryptoNote::BlockOffset rhs) {
  return lhs = BlockOffset::fromNative(lhs.native() - rhs.native());
}
