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

#include <memory>
#include <chrono>

namespace Xi {
namespace Http {
class ClientSession;

/*!
 * \brief The IClientSessionBuilder class abstract the way new client sessions are builded.
 */
class IClientSessionBuilder {
 public:
  IClientSessionBuilder() = default;
  virtual ~IClientSessionBuilder() = default;

  virtual std::shared_ptr<ClientSession> makeHttpSession() = 0;
  virtual std::shared_ptr<ClientSession> makeHttpsSession() = 0;
  virtual std::chrono::seconds timeout() const = 0;
};

}  // namespace Http
}  // namespace Xi
