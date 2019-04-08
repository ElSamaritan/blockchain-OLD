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

#include <Xi/Byte.h>

#include "Xi/Config/_Impl/BeginMinerReward.h"

/*!
 * \section Reward
 *
 * Index  : Chronological order of introduced checkpoints
 * Version: The block major version introducing the checkpoint
 * Window : Number of previous blocks for median size calculation, blocks with used to calculate penalities for larger
 *            blocks.
 * Zone   : Size in bytes until block penalties will be introduced. If a block is mined larger than the zone
 * size the base reward will be adjusted. Further the transaction pool won't accept transactions larger than the zone,
 *           except for fusion transactions.
 */

// clang-format off
//                  (_Index, _Version, _Window,    _Zone)
MakeRewardCheckpoint(     0,        1,     128,    64_kB)
// clang-format on

#define CURRENT_REWARD_CHECKPOINT_INDEX 0

#include "Xi/Config/_Impl/EndMinerReward.h"
