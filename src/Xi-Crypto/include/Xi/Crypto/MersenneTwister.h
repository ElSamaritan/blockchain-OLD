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

#include <cinttypes>

namespace Xi {
namespace Crypto {
/*!
 * \brief The MersenneTwister class implements a pseudo random generator for consistent random values accross diefferent
 * platforms.
 */
class MersenneTwister {
 private:
  static const uint32_t N = 624;
  static const uint32_t M = 397;
  static const uint32_t MATRIX_A = 0x9908b0df;
  static const uint32_t UPPER_MASK = 0x80000000;
  static const uint32_t LOWER_MASK = 0x7fffffff;
  static const uint32_t TEMPERING_MASK_B = 0x9d2c5680;
  static const uint32_t TEMPERING_MASK_C = 0xefc60000;

  const uint32_t mag01[2] = {0x0, MATRIX_A};

  inline uint32_t TEMPERING_SHIFT_U(uint32_t y) { return (y >> 11); }

  inline uint32_t TEMPERING_SHIFT_S(uint32_t y) { return (y << 7); }

  inline uint32_t TEMPERING_SHIFT_T(uint32_t y) { return (y << 15); }

  inline uint32_t TEMPERING_SHIFT_L(uint32_t y) { return (y >> 18); }

  uint32_t mt[N] = {0};
  uint16_t mti = 0;

 public:
  /*!
   * \brief MersenneTwister construct a new generator using the given seed
   */
  MersenneTwister(uint32_t seed) { setSeed(seed); }

  /*!
   * \brief setSeed sets the underlying seed. The implementation will always return the same sequence of bytes for the
   * same seed.
   * \param seed the seed to be applied
   */
  void setSeed(uint32_t seed) {
    mt[0] = seed & 0xffffffff;
    for (mti = 1; mti < N; mti++) mt[mti] = (69069 * mt[mti - 1]) & 0xffffffff;
  }

  /*!
   * \brief generateUnit generates the next atomic unit
   * \return a random distributed 32bit integer
   */
  uint32_t generateUnit() {
    uint32_t y;

    if (mti >= N) {
      uint32_t kk;

      for (kk = 0; kk < N - M; kk++) {
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1];
      }

      for (; kk < N - 1; kk++) {
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1];
      }

      y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
      mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1];

      mti = 0;
    }

    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    return y;
  }

  /*!
   * \brief next generates a random value in the range [min, max]
   * \param min the minimum value to be generated
   * \param max the maximum value to be generated
   * \return random value in [min, max]
   */
  uint32_t next(uint32_t min, uint32_t max) {
    uint32_t r = generateUnit();
    double div = (double)(0xffffffff) / (double)(max - min);
    return static_cast<uint32_t>(r / div) + min;
  }

  /*!
   * \brief next creates a new random value in the range [0, count)
   * \param count the exclusive upper limit
   * \return random value in [0, count)
   */
  uint32_t next(uint32_t count) { return next(0, count - 1); }

  /*!
   * \brief nextBytes generates a sequence of random bytes
   * \param data the output buffer to store the sequence
   * \param length the amount of bytes to be generated
   */
  void nextBytes(uint8_t* data, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) data[i] = (uint8_t)(generateUnit() / (0xffffffff / (uint32_t)0xff));
  }
};
}  // namespace Crypto
}  // namespace Xi
