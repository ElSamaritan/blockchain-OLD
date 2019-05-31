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

#include "CryptoNoteCore/Blockchain/BlockHeight.hpp"

#include <type_traits>

#include <Xi/Exceptions.hpp>

const CryptoNote::BlockHeight CryptoNote::BlockHeight::Null{std::numeric_limits<uint32_t>::max()};
const CryptoNote::BlockHeight CryptoNote::BlockHeight::Genesis{1};

CryptoNote::BlockHeight::BlockHeight(CryptoNote::BlockHeight::value_type height) : m_height{height} {}

CryptoNote::BlockHeight CryptoNote::BlockHeight::fromIndex(CryptoNote::BlockHeight::value_type index) {
  XI_RETURN_EC_IF_NOT(index < std::numeric_limits<value_type>::max(), Null);
  return BlockHeight{index + 1};
}

CryptoNote::BlockHeight CryptoNote::BlockHeight::fromNative(CryptoNote::BlockHeight::value_type native) {
  if (native < min() || native > max()) {
    return BlockHeight::Null;
  } else {
    return BlockHeight{native};
  }
}

CryptoNote::BlockHeight CryptoNote::BlockHeight::fromSize(size_t native) {
  if (native > max() || native < min()) {
    return BlockHeight::Null;
  } else {
    return BlockHeight::fromNative(static_cast<value_type>(native));
  }
}

CryptoNote::BlockHeight::BlockHeight() : m_height{Null.native()} {}

void CryptoNote::BlockHeight::displace(signed_value_type offset) {
  if (isNull()) {
    return;
  }

  int64_t signedHeight = m_height;
  int64_t signedOffset = offset;

  if (signedHeight + signedOffset > max() || signedHeight + signedOffset < min()) {
    *this = BlockHeight::Null;
  } else {
    m_height = static_cast<value_type>(signedHeight + signedOffset);
  }
}

void CryptoNote::BlockHeight::advance(size_t offset) {
  if (isNull()) {
    return;
  }

  if (offset > max()) {
    *this = BlockHeight::Null;
    return;
  }

  int64_t signedHeight = m_height;
  int64_t signedOffset = static_cast<uint32_t>(offset);

  if (signedHeight + signedOffset > max()) {
    *this = BlockHeight::Null;
  } else {
    m_height = static_cast<value_type>(signedHeight + signedOffset);
  }
}

CryptoNote::BlockHeight CryptoNote::BlockHeight::shift(CryptoNote::BlockHeight::signed_value_type offset) const {
  BlockHeight reval = *this;
  reval.displace(offset);
  return reval;
}

CryptoNote::BlockHeight CryptoNote::BlockHeight::next(size_t offset) const {
  BlockHeight reval = *this;
  reval.advance(offset);
  return reval;
}

CryptoNote::BlockHeight::value_type CryptoNote::BlockHeight::native() const { return m_height; }

CryptoNote::BlockHeight::value_type CryptoNote::BlockHeight::toIndex() const {
  XI_RETURN_EC_IF_NOT(m_height > 0, std::numeric_limits<value_type>::max());
  return m_height - 1;
}

CryptoNote::BlockHeight::value_type CryptoNote::BlockHeight::toSize() const { return m_height; }

bool CryptoNote::BlockHeight::isNull() const { return *this == BlockHeight::Null; }

bool CryptoNote::BlockHeight::operator==(const CryptoNote::BlockHeight rhs) const { return m_height == rhs.m_height; }
bool CryptoNote::BlockHeight::operator!=(const CryptoNote::BlockHeight rhs) const { return m_height != rhs.m_height; }
bool CryptoNote::BlockHeight::operator<(const CryptoNote::BlockHeight rhs) const { return m_height < rhs.m_height; }
bool CryptoNote::BlockHeight::operator<=(const CryptoNote::BlockHeight rhs) const { return m_height <= rhs.m_height; }
bool CryptoNote::BlockHeight::operator>(const CryptoNote::BlockHeight rhs) const { return m_height > rhs.m_height; }
bool CryptoNote::BlockHeight::operator>=(const CryptoNote::BlockHeight rhs) const { return m_height >= rhs.m_height; }

CryptoNote::BlockOffset CryptoNote::operator-(const CryptoNote::BlockHeight lhs, const CryptoNote::BlockHeight rhs) {
  int64_t lhsSigned = lhs.native();
  int64_t rhsSigned = rhs.native();
  return BlockOffset::fromNative(lhsSigned - rhsSigned);
}

CryptoNote::BlockHeight &CryptoNote::operator-=(CryptoNote::BlockHeight &lhs, const CryptoNote::BlockOffset rhs) {
  if (lhs.isNull()) {
    return lhs;
  }
  int64_t lhsSigned = lhs.native();
  int64_t rhsSigned = rhs.native();
  int64_t newHeight = lhsSigned - rhsSigned;
  if (newHeight < BlockHeight::min() || newHeight > BlockHeight::max()) {
    return lhs = BlockHeight::Null;
  } else {
    return lhs = BlockHeight::fromNative(static_cast<BlockHeight::value_type>(lhsSigned - rhsSigned));
  }
}

CryptoNote::BlockHeight &CryptoNote::operator+=(CryptoNote::BlockHeight &lhs, const CryptoNote::BlockOffset rhs) {
  if (lhs.isNull()) {
    return lhs;
  }
  int64_t lhsSigned = lhs.native();
  int64_t rhsSigned = rhs.native();
  int64_t newHeight = lhsSigned + rhsSigned;
  if (newHeight < BlockHeight::min() || newHeight > BlockHeight::max()) {
    return lhs = BlockHeight::Null;
  } else {
    return lhs = BlockHeight::fromNative(static_cast<BlockHeight::value_type>(lhsSigned + rhsSigned));
  }
}

CryptoNote::BlockHeight CryptoNote::operator-(const CryptoNote::BlockHeight &lhs, const CryptoNote::BlockOffset rhs) {
  BlockHeight reval{lhs};
  reval -= rhs;
  return reval;
}

CryptoNote::BlockHeight CryptoNote::operator+(const CryptoNote::BlockHeight &lhs, const CryptoNote::BlockOffset rhs) {
  BlockHeight reval{lhs};
  reval += rhs;
  return reval;
}

std::size_t std::hash<CryptoNote::BlockHeight>::operator()(const CryptoNote::BlockHeight offset) const {
  return std::hash<CryptoNote::BlockHeight::value_type>{}(offset.native());
}

std::string CryptoNote::toString(const CryptoNote::BlockHeight height) {
  if (height.isNull()) {
    return "null";
  } else {
    return std::to_string(height.native());
  }
}

bool CryptoNote::serialize(CryptoNote::BlockHeight &height, Common::StringView name,
                           CryptoNote::ISerializer &serializer) {
  uint32_t nativeHeight = 0;
  if (serializer.isOutput()) {
    if (height.isNull()) {
      nativeHeight = 0;
    } else if (height.native() < BlockHeight::min() || height.native() > BlockHeight::max()) {
      return false;
    } else {
      nativeHeight = height.native();
    }
  }

  XI_RETURN_EC_IF_NOT(serializer(nativeHeight, name), false);

  if (serializer.isInput()) {
    if (nativeHeight == 0) {
      height = BlockHeight::Null;
    } else if (nativeHeight < BlockHeight::min() || nativeHeight > BlockHeight::max()) {
      return false;
    } else {
      height = BlockHeight::fromNative(nativeHeight);
    }
  }

  return true;
}
