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

#include <Xi/Byte.hh>

namespace Xi {
namespace Config {
namespace Database {
inline std::string blocksFilename() { return "blocks"; }
inline std::string blockIndicesFilename() { return "block-indices"; }
inline std::string pooldataFilename() { return "transaction-pool"; }

inline constexpr uint64_t writeBufferSize() { return 256_MB; }
inline constexpr uint64_t readBufferSize() { return 10_MB; }
inline constexpr uint16_t maximumOpenFiles() { return 100; }
inline constexpr uint16_t backgroundThreads() { return 2; }
}  // namespace Database
}  // namespace Config
}  // namespace Xi
