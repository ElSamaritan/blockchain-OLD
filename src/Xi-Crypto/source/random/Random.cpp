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

#include "Xi/Crypto/Random/Random.hh"

#include <utility>

#include <Xi/Global.hh>
#include <Xi/Exceptions.hpp>

Xi::Result<Xi::ByteVector> Xi::Crypto::Random::generate(size_t count) {
  Xi::ByteVector reval;
  reval.resize(count);
  if (const auto ec = generate(ByteSpan{reval.data(), count}); ec != RandomError::Success) {
    return makeError(ec);
  }
  return success(std::move(reval));
}

Xi::Crypto::Random::RandomError Xi::Crypto::Random::generate(Xi::ByteSpan out) {
  XI_RETURN_EC_IF(xi_crypto_random_bytes(out.data(), out.size()) != XI_RETURN_CODE_SUCCESS, RandomError::Failed);
  return RandomError::Success;
}

Xi::Result<Xi::ByteVector> Xi::Crypto::Random::generate(size_t count, ConstByteSpan seed) {
  Xi::ByteVector reval;
  reval.resize(count);
  if (const auto ec = generate(ByteSpan{reval.data(), count}, seed); ec != RandomError::Success) {
    return makeError(ec);
  }
  return success(std::move(reval));
}

Xi::Crypto::Random::RandomError Xi::Crypto::Random::generate(ByteSpan out, ConstByteSpan seed) {
  XI_RETURN_EC_IF(
      xi_crypto_random_bytes_determenistic(out.data(), out.size(), seed.data(), seed.size()) != XI_RETURN_CODE_SUCCESS,
      RandomError::Failed);
  return RandomError::Success;
}
