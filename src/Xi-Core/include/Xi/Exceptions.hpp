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

#include "Xi/Exceptional.hpp"

namespace Xi {
XI_DECLARE_EXCEPTIONAL_CATEGORY(Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(Runtime, "generic runtime error", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(IndexOutOfRange, "array/vector index is out of bounds", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(OutOfRange, "number is out of range, usually required for a cast", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidIndex, "array/vector index is invalid in called context", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidSize, "array/vector size is invalid", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidVariantType, "unexpected type in variant", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidEnumValue, "enum does not match any decleration", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(Format, "invalid format for conversion", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(NotImplemented, "feature required is not implemented yet", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(NotFound, "an object was requested but not found", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidArgument, "argument provided is invalid", Runtime)
XI_DECLARE_EXCEPTIONAL_INSTANCE(NotInitialized, "object is not initialized and cannot perform yet", Runtime)
}  // namespace Xi
