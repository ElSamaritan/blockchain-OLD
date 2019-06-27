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

#if defined(__cplusplus)
extern "C" {
#endif

/// Specifies an endianess type at runtime.
enum xi_endianess_type {
  /// Indicates no endianess conversion should be applied.
  Xi_Endianess_Type_Native = 1,
  /// Little endiannes conversion is applied.
  Xi_Endianess_Type_Little = 2,
  /// Big endiannes conversion is applied.
  Xi_Endianess_Type_Big = 3,
};

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

namespace Xi {
namespace Endianess {

/// Specifies an endianess type at runtime.
enum struct Type {
  /// Indicates no endianess conversion should be applied.
  Native = Xi_Endianess_Type_Native,
  /// Little endiannes conversion is applied.
  Little = Xi_Endianess_Type_Little,
  /// Big endiannes conversion is applied.
  Big = Xi_Endianess_Type_Big,
};

}  // namespace Endianess
}  // namespace Xi

#endif
