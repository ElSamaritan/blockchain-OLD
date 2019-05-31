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

#pragma once

#include <type_traits>
#include <limits>
#include <memory>
#include <cinttypes>

#include <Xi/Global.hh>
#include <Xi/Byte.hh>

#include "Xi/Crypto/Random/Random.hh"

namespace Xi {
namespace Crypto {
namespace Random {

class Engine32 final {
 public:
  using result_type = std::uint_fast32_t;
  using seed_type = ConstByteSpan;

  static inline constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
  static inline constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

  explicit Engine32();
  explicit Engine32(seed_type seed);
  XI_DELETE_COPY(Engine32);
  XI_DEFAULT_MOVE(Engine32);
  ~Engine32();

  void seed();
  void seed(seed_type seed);

  result_type operator()();
  void operator()(ByteSpan out);

 private:
  std::unique_ptr<xi_crypto_random_state, void (*)(xi_crypto_random_state*)> m_state;
};

class Engine64 final {
 public:
  using result_type = std::uint_fast64_t;
  using seed_type = ConstByteSpan;

  static inline constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
  static inline constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

  explicit Engine64();
  explicit Engine64(seed_type seed);
  XI_DELETE_COPY(Engine64);
  XI_DEFAULT_MOVE(Engine64);
  ~Engine64();

  void seed();
  void seed(seed_type seed);

  result_type operator()();
  void operator()(ByteSpan out);

 private:
  std::unique_ptr<xi_crypto_random_state, void (*)(xi_crypto_random_state*)> m_state;
};

}  // namespace Random
}  // namespace Crypto
}  // namespace Xi
