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
 * ============================================================================================== *
 * Attribution:                                                                                   *
 *                                                                                                *
 * Thanks to cslarsen for his implementation on github.                                           *
 *    https://github.com/cslarsen/mersenne-twister                                                *
 * ============================================================================================== */

#include "Xi/Crypto/MersenneTwister.h"

#include <cstdint>
#include <cstring>
#include <algorithm>

#define SIZE 624u
#define PERIOD 397u
#define MAGIC 0x9908b0df
#define DIFF (SIZE - PERIOD)
#define M32(x) (0x80000000 & x)  // 32nd MSB
#define L31(x) (0x7FFFFFFF & x)  // 31 LSBs

#define UNROLL(expr)                                                                    \
  y = M32(m_State->MT[i]) | L31(m_State->MT[i + 1]);                                    \
  m_State->MT[i] = m_State->MT[expr] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & MAGIC); \
  ++i;

struct Xi::Crypto::MersenneTwister::_State {
  uint32_t MT[SIZE];
  uint32_t MT_TEMPERED[SIZE];
  uint32_t index = SIZE;
};

Xi::Crypto::MersenneTwister::MersenneTwister(uint32_t seed) : m_State{new _State} { setSeed(seed); }

Xi::Crypto::MersenneTwister::~MersenneTwister() {}

void Xi::Crypto::MersenneTwister::setSeed(uint32_t seed) {
  m_State->MT[0] = seed;
  m_State->index = SIZE;

  for (uint_fast32_t i = 1; i < SIZE; ++i) {
    m_State->MT[i] = 0x6c078965 * (m_State->MT[i - 1] ^ m_State->MT[i - 1] >> 30) + i;
  }
}

uint32_t Xi::Crypto::MersenneTwister::generateUnit() {
  if (m_State->index == SIZE) fillBuffer();
  return m_State->MT_TEMPERED[m_State->index++];
}

uint32_t Xi::Crypto::MersenneTwister::next(uint32_t min, uint32_t max) {
  uint32_t r = generateUnit();
  double div = (double)(0xffffffff) / (double)(max - min);
  return static_cast<uint32_t>(r / div) + min;
}

uint32_t Xi::Crypto::MersenneTwister::next(uint32_t count) { return next(0, count - 1); }

void Xi::Crypto::MersenneTwister::nextBytes(uint8_t *data, uint32_t length) {
  uint32_t i = 0;
  while (i < length) {
    if (m_State->index == SIZE) fillBuffer();

    uint32_t bufferSize = SIZE - m_State->index;
    uint32_t stripe = std::min(bufferSize * 4, length - i);
    std::memcpy(data + i, reinterpret_cast<uint8_t *>(m_State->MT_TEMPERED + m_State->index), stripe);

    i += stripe;
    m_State->index += stripe / 4;
    if ((stripe / 4) * 4 < stripe) m_State->index++;
  }
}

void Xi::Crypto::MersenneTwister::fillBuffer() {
  /*
   * For performance reasons, we've unrolled the loop three times, thus
   * mitigating the need for any modulus operations. Anyway, it seems this
   * trick is old hat: http://www.quadibloc.com/crypto/co4814.htm
   */

  size_t i = 0;
  uint32_t y;

  // i = [0 ... 226]
  while (i < DIFF) {
    /*
     * We're doing 226 = 113*2, an even number of steps, so we can safely
     * unroll one more step here for speed:
     */
    UNROLL(i + PERIOD);
  }

  // i = [227 ... 622]
  while (i < SIZE - 1) {
    /*
     * 623-227 = 396 = 2*2*3*3*11, so we can unroll this loop in any number
     * that evenly divides 396 (2, 4, 6, etc). Here we'll unroll 11 times.
     */
    UNROLL(i - DIFF);
  }

  {
    // i = 623, last step rolls over
    y = M32(m_State->MT[SIZE - 1]) | L31(m_State->MT[0]);
    m_State->MT[SIZE - 1] = m_State->MT[PERIOD - 1] ^ (y >> 1) ^ (((int32_t(y) << 31) >> 31) & MAGIC);
  }

  // Temper all numbers in a batch
  for (size_t j = 0; j < SIZE; ++j) {
    y = m_State->MT[j];
    y ^= y >> 11;
    y ^= y << 7 & 0x9d2c5680;
    y ^= y << 15 & 0xefc60000;
    y ^= y >> 18;
    m_State->MT_TEMPERED[j] = y;
  }

  m_State->index = 0;
}
