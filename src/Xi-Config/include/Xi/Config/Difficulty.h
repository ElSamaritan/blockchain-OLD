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

#pragma once

#include <cinttypes>
#include <chrono>

#include <Xi/Utils/Conversion.h>

#include "Xi/Config/_Impl/BeginDifficulty.h"

/*!
 * Configuring LWMA
 *
 * _Index      : The order in which the different configurations apply.
 * _Version    : The block version when this algorithm should be applied
 * _Window     : The amount of previous blocks that shall be considered for calculating the new difficulty.
 * _Initial    : The initial difficulty chosen if not enough block have been mined to obtain all data required.
 * _Algorithm  : The implementation to use for difficulty calculation from that checkpoint on to the next, if present
 *
 */

// clang-format off
//                      (_Index, _Version, _Window, _Initial,                       _Algorithm)
MakeDifficultyCheckpoint(     0,        1,      64,     1000, ::CryptoNote::Difficulty::LWMA_3)
// clang-format on

#define CURRENT_DIFFICULTY_CHECKPOINT_INDEX 0

#include "Xi/Config/_Impl/EndDifficulty.h"
