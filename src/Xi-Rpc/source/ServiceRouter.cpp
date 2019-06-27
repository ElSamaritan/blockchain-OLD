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

#include "Xi/Rpc/ServiceRouter.hpp"

#include <Xi/Algorithm/String.h>

namespace Xi {
namespace Rpc {

bool ServiceRouter::addService(std::string_view prefix, SharedIServiceProvider service) {
  exceptional_if<InvalidArgumentError>(service.get() == nullptr);
  exceptional_if<InvalidArgumentError>(prefix.empty(), "invalid command");
  XI_CONCURRENT_LOCK_WRITE(m_guard);

  auto search = m_services.find(std::string{prefix});
  XI_RETURN_EC_IF_NOT(search == m_services.end(), false);
  m_services.insert(std::make_pair(std::string{prefix}, service));
  return true;
}

bool ServiceRouter::removeService(std::string_view prefix) {
  exceptional_if<InvalidArgumentError>(prefix.empty(), "invalid command");
  XI_CONCURRENT_LOCK_WRITE(m_guard);

  auto search = m_services.find(std::string{prefix});
  XI_RETURN_EC_IF(search == m_services.end(), false);
  m_services.erase(search);
  return true;
}

ServiceError ServiceRouter::process(std::string_view command, CryptoNote::ISerializer &input,
                                    CryptoNote::ISerializer &output) {
  XI_RETURN_EC_IF(command.empty(), ServiceError::NotFound);
  XI_CONCURRENT_LOCK_READ(m_guard);
  std::string cmd{command};
  for (const auto &service : m_services) {
    if (starts_with(cmd, service.first)) {
      cmd.erase(0, service.first.size());
      if (cmd.size() > 0 && cmd.front() == '.') {
        cmd.erase(0, 1);
      }
      return service.second->operator()(command, input, output);
    }
  }
  return ServiceError::NotFound;
}

}  // namespace Rpc
}  // namespace Xi
