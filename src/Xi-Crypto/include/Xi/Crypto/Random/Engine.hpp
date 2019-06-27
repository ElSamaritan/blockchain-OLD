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

#include <type_traits>
#include <limits>
#include <memory>
#include <cinttypes>

#include <Xi/Global.hh>
#include <Xi/Byte.hh>
#include <Xi/Exceptions.hpp>

#include "Xi/Crypto/Random/Random.hh"

namespace Xi {
namespace Crypto {
namespace Random {

template <typename _ValueT>
class Engine final {
  static_assert(std::is_integral_v<_ValueT>, "random engine is only supported for integralt types.");

 public:
  using result_type = _ValueT;
  using seed_type = ConstByteSpan;

  static inline constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
  static inline constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

 private:
  std::unique_ptr<xi_crypto_random_state, void (*)(xi_crypto_random_state*)> m_state;

  explicit Engine() : m_state{xi_crypto_random_state_create(), xi_crypto_random_state_destroy} {}

  XI_DELETE_COPY(Engine);

 public:
  static Result<Engine> create() {
    Engine reval{};
    XI_RETURN_EC_IF(reval.m_state.get() == nullptr, failure(RandomError::Failed));
    XI_RETURN_EC_IF(xi_crypto_random_state_init(reval.m_state.get()) != XI_RETURN_CODE_SUCCESS,
                    failure(RandomError::Failed));
    return success(std::move(reval));
  }

  XI_DEFAULT_MOVE(Engine);
  ~Engine() = default;

  RandomError seed() {
    XI_RETURN_EC_IF(xi_crypto_random_state_init(this->m_state.get()) != XI_RETURN_CODE_SUCCESS, RandomError::Failed);
    return RandomError::Success;
  }

  RandomError seed(seed_type seed) {
    XI_RETURN_EC_IF(xi_crypto_random_state_init_deterministic(this->m_state.get(), seed.data(), seed.size_bytes()) !=
                        XI_RETURN_CODE_SUCCESS,
                    RandomError::Failed);
    return RandomError::Success;
  }

  result_type operator()() {
    result_type reval;
    exceptional_if<RuntimeError>(
        xi_crypto_random_bytes_from_state_deterministic(reinterpret_cast<xi_byte_t*>(&reval), sizeof(reval),
                                                        this->m_state.get()) != XI_RETURN_CODE_SUCCESS,
        "runtime generation failed");
    return reval;
  }

  RandomError operator()(ByteSpan out) {
    XI_RETURN_EC_IF(xi_crypto_random_bytes_from_state_deterministic(out.data(), out.size_bytes(),
                                                                    this->m_state.get()) != XI_RETURN_CODE_SUCCESS,
                    RandomError::Failed);
    return RandomError::Success;
  }
};

}  // namespace Random
}  // namespace Crypto
}  // namespace Xi
