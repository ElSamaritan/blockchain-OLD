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

#include "crypto/cnx/cnx.h"

#include <vector>
#include <array>
#include <memory>
#include <random>
#include <algorithm>

#include <Xi/Crypto/MersenneTwister.h>

#include "crypto/keccak.h"
#include "crypto/aes-support.h"
#include "crypto/cnx/distribution.h"
#include "crypto/cnx/cnx-hash.h"
#include "crypto/hash-extra-ops.h"

void Crypto::CNX::Hash_v0::operator()(const void *data, size_t length, Crypto::Hash &hash,
                                      bool forceSoftwareAES) const {
  std::memset(&hash, 0, sizeof(hash));
  cn_fast_hash(data, length, reinterpret_cast<char *>(&hash));

  std::size_t accumulatedScratchpad = 0;
  for (std::size_t i = 0; accumulatedScratchpad < 3 * 128_kB; ++i) {
    uint32_t softShellIndex = get_soft_shell_index(*reinterpret_cast<uint32_t *>(&hash));
    const uint32_t offset = offsetForHeight(softShellIndex);
    const uint32_t scratchpadSize = scratchpadSizeForOffset(offset);
    int8_t flags = 0;
    if (!forceSoftwareAES && check_aes_hardware_support() && !check_aes_hardware_disabled())
      flags |= CNX_FLAGS_HARDWARE_AES;
    const cnx_hash_config config{scratchpadSize, scratchpadSize / 2, flags};
    cnx_hash((const uint8_t *)data, length, &config, hash.data);

    accumulatedScratchpad += scratchpadSize;
  }
}
