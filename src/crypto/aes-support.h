/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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
#endif  // defined(__cplusplus)

/*!
 * \brief check_aes_hardware_support queries the cpu information to check for built in aes support
 * \return true if the cpu has built in aes support
 */
int check_aes_hardware_support(void);

/*!
 * \brief check_aes_hardware_disabled checks wheter the environment variable XI_USE_SOFTWARE_AES is set
 * \return true if the environment variable XI_USE_SOFTWARE_AES is set, otherwise false
 */
int check_aes_hardware_disabled(void);

#if defined(__cplusplus)
}
#endif  // defined(__cplusplus)
