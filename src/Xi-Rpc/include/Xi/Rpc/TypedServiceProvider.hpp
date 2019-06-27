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

#include <type_traits>

#include <Serialization/ISerializer.h>

#include "Xi/Rpc/IServiceProvider.hpp"

namespace Xi {
namespace Rpc {

template <typename _RequestT, typename _ResponseT>
class TypedServiceProvider : public IServiceProvider {
 protected:
  using IServiceProvider::IServiceProvider;

 public:
  virtual ~TypedServiceProvider() override = default;

 protected:
  virtual _RequestT makeDefaultRequest() { return _RequestT{}; }
  virtual _ResponseT makeDefaultResponse() { return _ResponseT{}; }

  virtual ServiceError process(std::string_view command, const _RequestT& request, _ResponseT& response) = 0;
  ServiceError process(std::string_view command, CryptoNote::ISerializer& input,
                       CryptoNote::ISerializer& output) override {
    if (!input.isInput() || !output.isOutput()) {
      return ServiceError::SerializationError;
    }

    auto request = this->makeDefaultRequest();
    if (!input(const_cast<_RequestT&>(request), "")) {
      return ServiceError::InvalidArgument;
    }

    auto response = this->makeDefaultResponse();
    const auto reval = this->process(command, request, response);
    if (reval != ServiceError::Success) {
      return reval;
    }

    if (!output(response, "")) {
      return ServiceError::SerializationError;
    }

    return ServiceError::Success;
  }
};

}  // namespace Rpc
}  // namespace Xi
