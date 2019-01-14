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

#include "Xi/Config/_Impl/BeginMixinConfig.h"

/*!
 * Creating a new Mixin Checkpoint
 *
 * Copy one of the MakeCheckpoint lines below to the bottom of the listing. Adjust the index to the previous index
 * plus one. Finally increment the CURRENT_MIXIN_CHECKPOINT_INDEX definition below the list to the newly introduced
 * index.
 *
 * There are no further changes to the code required. If you changed the code for a running blockchain you may want
 * to add the choosen height to the forks array to notify running daemons about the upcomming fork.
 *
 */

// clang-format off
//                 (_Index, _Version, _Min, _Max, _Default)
MakeMixinCheckpoint(     0,        1,    0,    3,        0)
// clang-format on

#define CURRENT_MIXIN_CHECKPOINT_INDEX 0

#include "Xi/Config/_Impl/EndMixinConfig.h"
