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

#include <exception>
#include <memory>
#include <string_view>

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>
#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>

#include "Xi/Rpc/ServiceError.hpp"

namespace Xi {
namespace Rpc {

class IServiceProvider {
 protected:
  IServiceProvider(Logging::ILogger& logger);

 public:
  virtual ~IServiceProvider() = default;

  ServiceError operator()(std::string_view command, CryptoNote::ISerializer& input, CryptoNote::ISerializer& output);

 protected:
  [[nodiscard]] virtual ServiceError process(std::string_view command, CryptoNote::ISerializer& input,
                                             CryptoNote::ISerializer& output) = 0;

 private:
  void logThrow(std::exception& e);

 protected:
  Logging::LoggerRef m_logger;
};

using SharedIServiceProvider = std::shared_ptr<IServiceProvider>;
using WeakIServiceProvider = std::weak_ptr<IServiceProvider>;

}  // namespace Rpc
}  // namespace Xi
