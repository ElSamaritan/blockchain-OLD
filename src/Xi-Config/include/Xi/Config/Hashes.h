﻿/* ============================================================================================== *
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

#include <CryptoNoteCore/HashAlgorithms.h>

#include "Xi/Config/_Impl/BeginHashes.h"

// clang-format off
//                (_Index, _Version,                            _Algorithm)
MakeHashCheckpoint(     0,        1,  ::CryptoNote::Hashes::InitBlockchain)
MakeHashCheckpoint(     1,        2,  ::CryptoNote::Hashes::CNX_v0        )
// clang-format on

#define CURRENT_HASH_CHECKPOINT_INDEX 1

#include "Xi/Config/_Impl/EndHashes.h"