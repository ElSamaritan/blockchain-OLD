﻿/* ============================================================================================== *
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

#include <Serialization/ISerializer.h>

namespace CryptoNote {
namespace RpcCommands {

struct SubmitBlock {
  static inline std::string identifier() {
    static const std::string __Identifier{"submit_block"};
    return __Identifier;
  }

  struct request {
    std::string hex_binary_template;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(hex_binary_template);
    KV_END_SERIALIZATION
  };

  struct response {
    std::string status;
    std::string result;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(result)
    KV_END_SERIALIZATION
  };
};

}  // namespace RpcCommands
}  // namespace CryptoNote
