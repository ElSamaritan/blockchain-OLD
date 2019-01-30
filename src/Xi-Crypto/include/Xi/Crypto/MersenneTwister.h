/* ============================================================================================== *
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

#include <cinttypes>
#include <memory>

namespace Xi {
namespace Crypto {
/*!
 * \brief The MersenneTwister class implements a pseudo random generator for consistent random values accross diefferent
 * platforms.
 */
class MersenneTwister {
 public:
  /*!
   * \brief MersenneTwister construct a new generator using the given seed
   */
  MersenneTwister(uint32_t seed);
  ~MersenneTwister();

  /*!
   * \brief setSeed sets the underlying seed. The implementation will always return the same sequence of bytes for the
   * same seed.
   * \param seed the seed to be applied
   */
  void setSeed(uint32_t seed);

  /*!
   * \brief generateUnit generates the next atomic unit
   * \return a random distributed 32bit integer
   */
  uint32_t generateUnit();

  /*!
   * \brief next generates a random value in the range [min, max]
   * \param min the minimum value to be generated
   * \param max the maximum value to be generated
   * \return random value in [min, max]
   */
  uint32_t next(uint32_t min, uint32_t max);

  /*!
   * \brief next creates a new random value in the range [0, count)
   * \param count the exclusive upper limit
   * \return random value in [0, count)
   */
  uint32_t next(uint32_t count);

  /*!
   * \brief nextBytes generates a sequence of random bytes
   * \param data the output buffer to store the sequence
   * \param length the amount of bytes to be generated
   */
  void nextBytes(uint8_t* data, uint32_t length);

 private:
  void fillBuffer();

  struct _State;
  std::unique_ptr<_State> m_State;
};
}  // namespace Crypto
}  // namespace Xi
