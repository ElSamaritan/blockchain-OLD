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

#include <Xi/ErrorCode.hpp>

namespace Xi {
namespace Rpc {

XI_ERROR_CODE_BEGIN(Service)

/// Should be returned if an exceptional behaviour occured, usually indicates an internal error
XI_ERROR_CODE_VALUE(Exceptional, 0x0001)
/// The operation timed out, you may not need to handle this yourself
XI_ERROR_CODE_VALUE(Timeout, 0x0002)

XI_ERROR_CODE_VALUE(NotInitialized, 0x0003)

XI_ERROR_CODE_VALUE(InvalidArgument, 0x0004)

XI_ERROR_CODE_VALUE(SerializationError, 0x0005)

XI_ERROR_CODE_VALUE(NotFound, 0x0006)

XI_ERROR_CODE_END(Service, "service operation error")

}  // namespace Rpc
}  // namespace Xi

XI_ERROR_CODE_OVERLOADS(Xi::Rpc, Service)
