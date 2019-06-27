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

#include "Xi/Rpc/ServiceProviderCollection.hpp"

#include <Xi/Exceptions.hpp>
#include <Xi/Algorithm/String.h>

namespace Xi {
namespace Rpc {

ServiceProviderCollection::ServiceProviderCollection(Logging::ILogger &logger) : IServiceProvider(logger) {}

const std::string &ServiceProviderCollection::prefix() const {
  XI_CONCURRENT_LOCK_READ(m_guard);
  return m_prefix;
}

void ServiceProviderCollection::setPrefix(std::string _prefix) {
  XI_CONCURRENT_LOCK_WRITE(m_guard);
  m_prefix = _prefix;
}

bool ServiceProviderCollection::addService(std::string_view command, SharedIServiceProvider service) {
  exceptional_if<InvalidArgumentError>(service.get() == nullptr);
  exceptional_if<InvalidArgumentError>(command.empty(), "invalid command");
  XI_CONCURRENT_LOCK_WRITE(m_guard);

  auto search = m_services.find(std::string{command});
  XI_RETURN_EC_IF_NOT(search == m_services.end(), false);
  m_services.insert(std::make_pair(std::string{command}, service));
  return true;
}

bool ServiceProviderCollection::removeService(std::string_view command) {
  exceptional_if<InvalidArgumentError>(command.empty(), "invalid command");
  XI_CONCURRENT_LOCK_WRITE(m_guard);

  auto search = m_services.find(std::string{command});
  XI_RETURN_EC_IF(search == m_services.end(), false);
  m_services.erase(search);
  return true;
}

ServiceError ServiceProviderCollection::process(std::string_view command, CryptoNote::ISerializer &input,
                                                CryptoNote::ISerializer &output) {
  XI_RETURN_EC_IF(command.empty(), ServiceError::NotFound);
  XI_CONCURRENT_LOCK_READ(m_guard);
  std::string scommand{command};
  if (!m_prefix.empty()) {
    if (!starts_with(scommand, m_prefix + ".")) {
      return ServiceError::NotFound;
    } else {
      scommand.erase(0, m_prefix.size() + 1);
    }
  }

  auto search = m_services.find(scommand);
  XI_RETURN_EC_IF(search == m_services.end(), ServiceError::NotFound);
  return search->second->operator()(command, input, output);
}

}  // namespace Rpc
}  // namespace Xi
