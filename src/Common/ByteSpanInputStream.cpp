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

#include "Common/ByteSpanInputStream.hpp"

#include <algorithm>
#include <cstring>

Common::ByteSpanInputStream::ByteSpanInputStream(Xi::ConstByteSpan input) : m_input{input} {
}

size_t Common::ByteSpanInputStream::readSome(void *data, size_t size) {
  const auto maxSize = std::min(size, m_input.size_bytes());
  std::memcpy(data, m_input.data(), maxSize);
  m_input = m_input.slice(maxSize);
  return maxSize;
}

bool Common::ByteSpanInputStream::isEndOfStream() const {
  return m_input.size() == 0;
}
