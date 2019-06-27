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

#include <map>

#include <Xi/Global.hh>
#include <Xi/Concurrent/ReadersWriterLock.h>

#include "Xi/Rpc/IServiceProvider.hpp"

namespace Xi {
namespace Rpc {

class ServiceRouter : public IServiceProvider {
 public:
  using IServiceProvider::IServiceProvider;
  ~ServiceRouter() override = default;

  bool addService(std::string_view prefix, SharedIServiceProvider service);
  bool removeService(std::string_view prefix);

 protected:
  [[nodiscard]] virtual ServiceError process(std::string_view command, CryptoNote::ISerializer& input,
                                             CryptoNote::ISerializer& output) override;

 private:
  Concurrent::ReadersWriterLock m_guard;

  /// (prefix, service)
  std::map<std::string, SharedIServiceProvider> m_services;
};

}  // namespace Rpc
}  // namespace Xi
