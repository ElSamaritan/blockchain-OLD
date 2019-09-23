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

#include <Xi/ErrorCode.hpp>

namespace Xi {
namespace Mnemonic {

// clang-format off
XI_ERROR_CODE_BEGIN(Mnemonic)

InvalidEntropy = 0x0001,
WordNotFound = 0x0002,
InvalidChecksum = 0x0003,
UnexpectedSize = 0x0004,

XI_ERROR_CODE_END(Mnemonic, "mnemonic encoding failure")
// clang-format on

}  // namespace Mnemonic
}  // namespace Xi

XI_ERROR_CODE_OVERLOADS(Xi::Mnemonic, Mnemonic)
