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

#include <Serialization/ISerializer.h>

#include "Rpc/Commands/EmptyRequest.h"

namespace CryptoNote {
namespace RpcCommands {

struct GetBlockTemplateState {
  static inline std::string identifier() {
    static const std::string __Identifier{"get_block_template_state"};
    return __Identifier;
  }

  using request = Null;

  struct response {
    std::string status;

    /*!
     * \brief template_state Hash unique to a previous call, iff a new requested block template would not change.
     */
    std::string template_state;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(template_state)
    KV_END_SERIALIZATION
  };
};

}  // namespace RpcCommands
}  // namespace CryptoNote
