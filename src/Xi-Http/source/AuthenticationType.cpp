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

#include "Xi/Http/AuthenticationType.h"

#include <stdexcept>

std::string Xi::to_string(Xi::Http::AuthenticationType status) {
  switch (status) {
    case (Xi::Http::AuthenticationType::Basic):
      return "Basic";
    case (Xi::Http::AuthenticationType::Bearer):
      return "Bearer";
    case (Xi::Http::AuthenticationType::Unsupported):
      return "Unsupported";
    default:
      throw std::runtime_error{"invalid authentication type provided"};
  }
}

namespace Xi {
template <>
Xi::Http::AuthenticationType lexical_cast<Xi::Http::AuthenticationType>(const std::string &value) {
  if (value == to_string(Xi::Http::AuthenticationType::Basic))
    return Xi::Http::AuthenticationType::Basic;
  if (value == to_string(Xi::Http::AuthenticationType::Bearer))
    return Xi::Http::AuthenticationType::Bearer;
  else
    return Xi::Http::AuthenticationType::Unsupported;
}
}  // namespace Xi
