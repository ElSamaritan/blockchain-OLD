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

#include "Xi/Rpc/ServiceError.hpp"

XI_ERROR_CODE_CATEGORY_BEGIN(Xi::Rpc, Service)
XI_ERROR_CODE_DESC(Exceptional, "service threw an exception")
XI_ERROR_CODE_DESC(Timeout, "service operation timed out")
XI_ERROR_CODE_DESC(NotInitialized, "service is not initialized")
XI_ERROR_CODE_DESC(InvalidArgument, "invalid argument provided to service")
XI_ERROR_CODE_DESC(SerializationError, "command serialization failed")
XI_ERROR_CODE_DESC(NotFound, "command not found")
XI_ERROR_CODE_CATEGORY_END()
