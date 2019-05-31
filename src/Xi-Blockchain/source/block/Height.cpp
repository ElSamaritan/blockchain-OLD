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

#include "Xi/Blockchain/Block/Height.hpp"

#include <type_traits>

#include <Xi/Exceptions.hpp>

const Xi::Blockchain::Block::Height Xi::Blockchain::Block::Height::Null{std::numeric_limits<uint32_t>::max()};
const Xi::Blockchain::Block::Height Xi::Blockchain::Block::Height::Genesis{1};

Xi::Blockchain::Block::Height::Height(Xi::Blockchain::Block::Height::value_type height) : m_height{height} {}

Xi::Blockchain::Block::Height Xi::Blockchain::Block::Height::min() { return Height{0}; }

Xi::Blockchain::Block::Height Xi::Blockchain::Block::Height::max() { return Height{50'000'000}; }

Xi::Blockchain::Block::Height Xi::Blockchain::Block::Height::fromIndex(
    Xi::Blockchain::Block::Height::value_type index) {
  XI_RETURN_EC_IF_NOT(index < std::numeric_limits<value_type>::max(), Null);
  return Height{index + 1};
}

Xi::Blockchain::Block::Height Xi::Blockchain::Block::Height::fromNative(
    Xi::Blockchain::Block::Height::value_type native) {
  if (native < min().native() || native > max().native()) {
    return Height::Null;
  } else {
    return Height{native};
  }
}

Xi::Blockchain::Block::Height Xi::Blockchain::Block::Height::fromSize(size_t native) {
  if (native > max().native() || native < min().native()) {
    return Height::Null;
  } else {
    return Height::fromNative(static_cast<value_type>(native));
  }
}

Xi::Blockchain::Block::Height::Height() : m_height{Null.native()} {}

void Xi::Blockchain::Block::Height::displace(signed_value_type offset) {
  if (isNull()) {
    return;
  }

  int64_t signedHeight = m_height;
  int64_t signedOffset = offset;

  if (signedHeight + signedOffset > max().native() || signedHeight + signedOffset < min().native()) {
    *this = Height::Null;
  } else {
    m_height = static_cast<value_type>(signedHeight + signedOffset);
  }
}

void Xi::Blockchain::Block::Height::advance(size_t offset) {
  if (isNull()) {
    return;
  }

  if (offset > max().native()) {
    *this = Height::Null;
    return;
  }

  int64_t signedHeight = m_height;
  int64_t signedOffset = static_cast<uint32_t>(offset);

  if (signedHeight + signedOffset > max().native()) {
    *this = Height::Null;
  } else {
    m_height = static_cast<value_type>(signedHeight + signedOffset);
  }
}

Xi::Blockchain::Block::Height Xi::Blockchain::Block::Height::shift(
    Xi::Blockchain::Block::Height::signed_value_type offset) const {
  Height reval = *this;
  reval.displace(offset);
  return reval;
}

Xi::Blockchain::Block::Height Xi::Blockchain::Block::Height::next(size_t offset) const {
  Height reval = *this;
  reval.advance(offset);
  return reval;
}

Xi::Blockchain::Block::Height::value_type Xi::Blockchain::Block::Height::native() const { return m_height; }

Xi::Blockchain::Block::Height::value_type Xi::Blockchain::Block::Height::toIndex() const {
  XI_RETURN_EC_IF_NOT(m_height > 0, std::numeric_limits<value_type>::max());
  return m_height - 1;
}

Xi::Blockchain::Block::Height::value_type Xi::Blockchain::Block::Height::toSize() const { return m_height; }

bool Xi::Blockchain::Block::Height::isNull() const { return *this == Height::Null; }

bool Xi::Blockchain::Block::Height::operator==(const Xi::Blockchain::Block::Height rhs) const {
  return m_height == rhs.m_height;
}
bool Xi::Blockchain::Block::Height::operator!=(const Xi::Blockchain::Block::Height rhs) const {
  return m_height != rhs.m_height;
}
bool Xi::Blockchain::Block::Height::operator<(const Xi::Blockchain::Block::Height rhs) const {
  return m_height < rhs.m_height;
}
bool Xi::Blockchain::Block::Height::operator<=(const Xi::Blockchain::Block::Height rhs) const {
  return m_height <= rhs.m_height;
}
bool Xi::Blockchain::Block::Height::operator>(const Xi::Blockchain::Block::Height rhs) const {
  return m_height > rhs.m_height;
}
bool Xi::Blockchain::Block::Height::operator>=(const Xi::Blockchain::Block::Height rhs) const {
  return m_height >= rhs.m_height;
}

Xi::Blockchain::Block::Offset Xi::Blockchain::Block::operator-(const Xi::Blockchain::Block::Height lhs,
                                                               const Xi::Blockchain::Block::Height rhs) {
  int64_t lhsSigned = lhs.native();
  int64_t rhsSigned = rhs.native();
  return Offset::fromNative(lhsSigned - rhsSigned);
}

Xi::Blockchain::Block::Height &Xi::Blockchain::Block::operator-=(Xi::Blockchain::Block::Height &lhs, const Offset rhs) {
  if (lhs.isNull()) {
    return lhs;
  }
  int64_t lhsSigned = lhs.native();
  int64_t rhsSigned = rhs.native();
  int64_t newHeight = lhsSigned - rhsSigned;
  if (newHeight < Height::min().native() || newHeight > Height::max().native()) {
    return lhs = Height::Null;
  } else {
    return lhs = Height::fromNative(static_cast<Height::value_type>(lhsSigned - rhsSigned));
  }
}

Xi::Blockchain::Block::Height &Xi::Blockchain::Block::operator+=(Xi::Blockchain::Block::Height &lhs,
                                                                 const Xi::Blockchain::Block::Offset rhs) {
  if (lhs.isNull()) {
    return lhs;
  }
  int64_t lhsSigned = lhs.native();
  int64_t rhsSigned = rhs.native();
  int64_t newHeight = lhsSigned + rhsSigned;
  if (newHeight < Height::min().native() || newHeight > Height::max().native()) {
    return lhs = Height::Null;
  } else {
    return lhs = Height::fromNative(static_cast<Height::value_type>(lhsSigned + rhsSigned));
  }
}

Xi::Blockchain::Block::Height Xi::Blockchain::Block::operator-(const Xi::Blockchain::Block::Height &lhs,
                                                               const Xi::Blockchain::Block::Offset rhs) {
  Height reval{lhs};
  reval -= rhs;
  return reval;
}

Xi::Blockchain::Block::Height Xi::Blockchain::Block::operator+(const Xi::Blockchain::Block::Height &lhs,
                                                               const Xi::Blockchain::Block::Offset rhs) {
  Height reval{lhs};
  reval += rhs;
  return reval;
}

std::size_t std::hash<Xi::Blockchain::Block::Height>::operator()(const Xi::Blockchain::Block::Height offset) const {
  return std::hash<Xi::Blockchain::Block::Height::value_type>{}(offset.native());
}

std::string Xi::Blockchain::Block::toString(const Xi::Blockchain::Block::Height height) {
  if (height.isNull()) {
    return "null";
  } else {
    return std::to_string(height.native());
  }
}

bool Xi::Blockchain::Block::serialize(Xi::Blockchain::Block::Height &height, Common::StringView name,
                                      CryptoNote::ISerializer &serializer) {
  uint32_t nativeHeight = 0;
  if (serializer.isOutput()) {
    if (height.isNull()) {
      nativeHeight = 0;
    } else if (height.native() < Height::min().native() || height.native() > Height::max().native()) {
      return false;
    } else {
      nativeHeight = height.native();
    }
  }

  XI_RETURN_EC_IF_NOT(serializer(nativeHeight, name), false);

  if (serializer.isInput()) {
    if (nativeHeight == 0) {
      height = Height::Null;
    } else if (nativeHeight < Height::min().native() || nativeHeight > Height::max().native()) {
      return false;
    } else {
      height = Height::fromNative(nativeHeight);
    }
  }

  return true;
}
