/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include "Xi/Config/_Impl/BeginDust.h"

// clang-format off
//                (_Index, _Version,   _Dust)
MakeDustCheckpoint(     0,        1,     100)
MakeDustCheckpoint(     1,        5,    1000)
MakeDustCheckpoint(     2,        6,   10000)

#define CURRENT_DUST_CHECKPOINT_INDEX 2

//                      (_Index, _Version,   _Dust)
MakeFusionDustCheckpoint(     0,        1,      10)
MakeFusionDustCheckpoint(     1,        6,     200)
// clang-format on

#define CURRENT_FUSION_DUST_CHECKPOINT_INDEX 1

#include "Xi/Config/_Impl/EndDust.h"
