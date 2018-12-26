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

#include <string>
#include <vector>
#include <stdexcept>

#include <boost/uuid/uuid.hpp>

#include <Xi/Utils/Conversion.h>

#include "Xi/Config/Limits.h"
#include "Xi/Config/NetworkType.h"

namespace Xi {
namespace Config {
namespace Network {
Type defaultNetworkType();
boost::uuids::uuid identifier(Type network);
std::vector<std::string> seedNodes(Type network);

static inline constexpr uint16_t p2pPort() { return 22868; }
static inline constexpr uint16_t rpcPort() { return 22869; }
static inline constexpr uint16_t pgPort() { return 38070; }

static inline constexpr uint64_t blockIdentifiersSynchronizationBatchSize() { return 500; }
static inline constexpr uint64_t blocksSynchronizationBatchSize() { return 100; }
}  // namespace Network
}  // namespace Config
}  // namespace Xi