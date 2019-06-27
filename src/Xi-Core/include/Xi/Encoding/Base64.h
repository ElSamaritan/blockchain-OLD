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

#include <string>

namespace Xi {
namespace Base64 {
/*!
 * \brief encode encodes a raw data block into a Bas64 encoded string
 * \param raw the binary data to encode
 * \return the Base64 encoding of the binary data
 */
std::string encode(const std::string& raw);

/*!
 * \brief decodes a Base64 encoded data string into its binary format
 * \param raw the Base64 encoded data block
 * \return the raw data blob thats was encoded by the given Base64 string
 */
std::string decode(const std::string& raw);
}  // namespace Base64
}  // namespace Xi
