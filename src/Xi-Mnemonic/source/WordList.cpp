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

#include "Xi/Mnemonic/WordList.hpp"

#include <utility>
#include <algorithm>

#include <Xi/Exceptions.hpp>
#include <Xi/Algorithm/Math.h>

#include "Xi/Mnemonic/MnemonicError.hpp"

namespace Xi {
namespace Mnemonic {

bool WordList::empty() const {
  return m_words.empty();
}

const std::string &WordList::operator[](const size_t index) const {
  return m_words.at(index);
}

Result<size_t> WordList::operator[](const std::string &word) const {
  const auto search = std::find(begin(m_words), end(m_words), word);
  XI_FAIL_IF(search == end(m_words), MnemonicError::WordNotFound)
  return success(static_cast<size_t>(std::distance(begin(m_words), search)));
}

size_t WordList::bits() const {
  return m_bits;
}

size_t WordList::count() const {
  return m_words.size();
}

WordList::WordList(std::vector<std::string> words) : m_words{move(words)} {
  XI_EXCEPTIONAL_IF(InvalidArgumentError, m_words.empty())
  XI_EXCEPTIONAL_IF(InvalidArgumentError, m_words.size() < 2)
  XI_EXCEPTIONAL_IF_NOT(InvalidArgumentError, isPowerOf2(m_words.size()))
  m_bits = log2(m_words.size());
}

}  // namespace Mnemonic
}  // namespace Xi
