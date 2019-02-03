﻿/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

#include <inttypes.h>

/*!
 * \brief allocate the scratch buffer using OS support for huge pages, if available
 *
 * This function tries to allocate the scratch buffer using a single "huge page" (instead of the usual 4KB page sizes)
 * to reduce TLB misses during the random accesses to the scratch buffer.  This is one of the important speed
 * optimizations needed to make CryptoNight faster.
 *
 * \param pageSize The required memory for the scratchpad
 * \return a thread local pointer to the memory allocated, with at least pageSize memory
 */
uint8_t* xi_hash_allocate_state(uint32_t pageSize);

/*!
 * \brief frees the thread local allocated scratch buffer
 */
void xi_hash_free_state(void);

#if defined(__cplusplus)
}
#endif  // __cplusplus