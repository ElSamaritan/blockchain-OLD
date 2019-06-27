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

#include "Xi/Blockchain/Block/Offset.hpp"

#include "Xi/Blockchain/Block/Height.hpp"

Xi::Blockchain::Block::Offset::Offset(Xi::Blockchain::Block::Offset::value_type value) : m_native{value} {}

Xi::Blockchain::Block::Offset Xi::Blockchain::Block::Offset::fromNative(
    const Xi::Blockchain::Block::Offset::value_type value) {
  return Offset{value};
}

Xi::Blockchain::Block::Offset::Offset() : m_native{0} {}

int64_t Xi::Blockchain::Block::Offset::native() const { return m_native; }

bool Xi::Blockchain::Block::Offset::operator==(const Xi::Blockchain::Block::Offset rhs) const {
  return m_native == rhs.m_native;
}
bool Xi::Blockchain::Block::Offset::operator!=(const Xi::Blockchain::Block::Offset rhs) const {
  return m_native != rhs.m_native;
}
bool Xi::Blockchain::Block::Offset::operator<(const Xi::Blockchain::Block::Offset rhs) const {
  return m_native < rhs.m_native;
}
bool Xi::Blockchain::Block::Offset::operator<=(const Xi::Blockchain::Block::Offset rhs) const {
  return m_native <= rhs.m_native;
}
bool Xi::Blockchain::Block::Offset::operator>(const Xi::Blockchain::Block::Offset rhs) const {
  return m_native > rhs.m_native;
}
bool Xi::Blockchain::Block::Offset::operator>=(const Xi::Blockchain::Block::Offset rhs) const {
  return m_native >= rhs.m_native;
}

size_t std::hash<Xi::Blockchain::Block::Offset>::operator()(const Xi::Blockchain::Block::Offset offset) const {
  return std::hash<Xi::Blockchain::Block::Offset::value_type>{}(offset.native());
}

Xi::Blockchain::Block::Offset Xi::Blockchain::Block::operator+(const Xi::Blockchain::Block::Offset lhs,
                                                               const Xi::Blockchain::Block::Offset rhs) {
  return Offset::fromNative(lhs.native() + rhs.native());
}

Xi::Blockchain::Block::Offset& Xi::Blockchain::Block::operator+=(Xi::Blockchain::Block::Offset& lhs,
                                                                 const Xi::Blockchain::Block::Offset rhs) {
  return lhs = Offset::fromNative(lhs.native() + rhs.native());
}

Xi::Blockchain::Block::Offset Xi::Blockchain::Block::operator-(const Xi::Blockchain::Block::Offset lhs,
                                                               const Xi::Blockchain::Block::Offset rhs) {
  return Offset::fromNative(lhs.native() - rhs.native());
}

Xi::Blockchain::Block::Offset& Xi::Blockchain::Block::operator-=(Xi::Blockchain::Block::Offset& lhs,
                                                                 const Xi::Blockchain::Block::Offset rhs) {
  return lhs = Offset::fromNative(lhs.native() - rhs.native());
}

bool Xi::Blockchain::Block::serialize(Xi::Blockchain::Block::Offset& value, Common::StringView name,
                                      CryptoNote::ISerializer& serializer) {
  if (serializer.isOutput()) {
    XI_RETURN_EC_IF(value.native() < -static_cast<int64_t>(Height::max().native()), false);
    XI_RETURN_EC_IF(value.native() >= static_cast<int64_t>(Height::max().native()), false);
  }

  XI_RETURN_EC_IF_NOT(serializer(value.m_native, name), false);

  if (serializer.isInput()) {
    XI_RETURN_EC_IF(value.native() < -static_cast<int64_t>(Height::max().native()), false);
    XI_RETURN_EC_IF(value.native() >= static_cast<int64_t>(Height::max().native()), false);
  }

  return true;
}
