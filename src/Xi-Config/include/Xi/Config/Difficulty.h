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

#include <cinttypes>
#include <array>
#include <string>

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>

#include "Xi/Config/VersionContainer.hpp"

namespace Xi {
namespace Config {
namespace Difficulty {

class Configuration {
 public:
  XI_PROPERTY(std::string, algorithm)
  XI_PROPERTY(uint32_t, windowSize)
  XI_PROPERTY(uint64_t, initialDifficulty)
  XI_PROPERTY(std::string, proofOfWorkAlgorithm)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(algorithm(), algorithm)
  KV_MEMBER_RENAME(windowSize(), window_size)
  KV_MEMBER_RENAME(initialDifficulty(), initial)
  KV_MEMBER_RENAME(proofOfWorkAlgorithm(), proof_of_work)
  KV_END_SERIALIZATION
};

using Container = VersionContainer<Difficulty::Configuration>;

}  // namespace Difficulty
}  // namespace Config
}  // namespace Xi
