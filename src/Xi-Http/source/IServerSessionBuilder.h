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

#include <memory>
#include <utility>

#include "ServerSession.h"

namespace Xi {
namespace Http {
class IServerSessionBuilder {
 public:
  virtual ~IServerSessionBuilder() = default;

  virtual std::shared_ptr<ServerSession> makeHttpServerSession(ServerSession::socket_t socket,
                                                               ServerSession::buffer_t buffer) = 0;
  virtual std::shared_ptr<ServerSession> makeHttpsServerSession(ServerSession::socket_t socket,
                                                                ServerSession::buffer_t buffer) = 0;
};
}  // namespace Http
}  // namespace Xi
