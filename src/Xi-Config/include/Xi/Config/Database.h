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

#include <string>

#include <Xi/Utils/Conversion.h>

namespace Xi {
namespace Config {
namespace Database {
inline std::string blocksFilename() { return "xi-blocks"; }
inline std::string blockIndicesFilename() { return "xi-blockindices"; }
inline std::string pooldataFilename() { return "xi-transaction-pool"; }

inline constexpr uint64_t writeBufferSize() { return 256_MB; }
inline constexpr uint64_t readBufferSize() { return 10_MB; }
inline constexpr uint16_t maximumOpenFiles() { return 100; }
inline constexpr uint16_t backgroundThreads() { return 2; }
}  // namespace Database
}  // namespace Config
}  // namespace Xi
