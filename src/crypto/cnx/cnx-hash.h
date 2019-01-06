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

#include <stddef.h>
#include <inttypes.h>

enum {
  CNX_HASH_SIZE = 32,         ///< The size in bytes of the generated hash
  CNX_INDIRECTIONS_MAX = 16,  ///< The maximum amount of stored 128bit scratchpad values used for indirections
  CNX_INDICES_SIZE = 256,     ///< The amount of random indices expected in the config
  CNX_INDICES_MAX = 128 / 8,  ///<< The maximum index expected (AES Block Size / Bits per Byte)
};

enum {
  CNX_FLAGS_HARDWARE_AES =
      1 << 0,  ///< Useds as a flag for the config to indicate hardware accelerated aes computation should be used.
};

typedef struct cnx_hash_config cnx_hash_config;
struct cnx_hash_config {
  uint32_t scratchpad_size;
  uint32_t iterations;
  int8_t flags;
};

void cnx_hash(const uint8_t* data, const size_t length, const cnx_hash_config* config, uint8_t* hash);

#if defined(__cplusplus)
}
#endif  // defined(__cplusplus)
