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
#include <vector>
#include <cinttypes>
#include <initializer_list>

#include <Xi/Global.hh>
#include <Xi/Result.h>

namespace Xi {
namespace Mnemonic {

class WordList {
 public:
  static const WordList ChineseSimplified;
  static const WordList ChineseTraditional;
  static const WordList Czech;
  static const WordList English;
  static const WordList French;
  static const WordList Italian;
  static const WordList Japanese;
  static const WordList Korean;
  static const WordList Spanish;

 public:
  bool empty() const;

  const std::string& operator[](const size_t index) const;
  Result<size_t> operator[](const std::string& word) const;

  /// 2^bits() == count()
  size_t bits() const;
  size_t count() const;

 private:
  explicit WordList(std::vector<std::string> words);
  XI_DELETE_COPY(WordList);
  XI_DELETE_MOVE(WordList);
  ~WordList() = default;

 private:
  std::vector<std::string> m_words;
  size_t m_bits;
};

}  // namespace Mnemonic
}  // namespace Xi
