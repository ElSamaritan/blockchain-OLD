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

class ServiceProviderCollection : public IServiceProvider {
 public:
  ServiceProviderCollection(Logging::ILogger& logger);
  XI_DELETE_COPY(ServiceProviderCollection);
  ~ServiceProviderCollection() override = default;

  const std::string& prefix() const;
  void setPrefix(std::string _prefix);

  bool addService(std::string_view command, SharedIServiceProvider service);
  bool removeService(std::string_view command);

 protected:
  [[nodiscard]] virtual ServiceError process(std::string_view command, CryptoNote::ISerializer& input,
                                             CryptoNote::ISerializer& output) override;

 private:
  Concurrent::ReadersWriterLock m_guard;
  std::string m_prefix;
  std::map<std::string, SharedIServiceProvider> m_services;
};

template <typename... _TypesT>
class GenericServiceProviderCollection : public _TypesT... {
 protected:
  GenericServiceProviderCollection(Logging::ILogger& logger) : _TypesT(logger)... {}
  ~GenericServiceProviderCollection() override = default;
};

}  // namespace Rpc
}  // namespace Xi
