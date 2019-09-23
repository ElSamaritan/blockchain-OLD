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

#include <vector>
#include <string>

#include <Xi/Global.hh>
#include <Xi/Error.h>
#include <Xi/Result.h>
#include <Xi/Byte.hh>

#include "Xi/Mnemonic/WordList.hpp"

namespace Xi {
namespace Mnemonic {

Result<std::string> encode(ConstByteSpan data, const WordList& wordlist = WordList::English);
Result<ByteVector> decode(const std::string& mnemonic, const WordList& wordlist = WordList::English);
Result<void> decode(const std::string& mnemonic, ByteSpan out, const WordList& wordlist = WordList::English);

}  // namespace Mnemonic
}  // namespace Xi
