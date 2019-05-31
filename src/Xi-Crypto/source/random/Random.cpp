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

#include "Xi/Crypto/Random/Random.hh"

#include <Xi/Global.hh>
#include <Xi/Exceptions.hpp>

Xi::ByteVector Xi::Crypto::Random::generate(size_t count) {
  Xi::ByteVector reval;
  reval.resize(count);
  generate(ByteSpan{reval.data(), count});
  return reval;
}

void Xi::Crypto::Random::generate(Xi::ByteSpan out) {
  exceptional_if<GenerationError>(xi_crypto_random_bytes(out.data(), out.size()) != XI_RETURN_CODE_SUCCESS);
}

Xi::ByteVector Xi::Crypto::Random::generate(size_t count, ConstByteSpan seed) {
  Xi::ByteVector reval;
  reval.resize(count);
  generate(ByteSpan{reval.data(), count}, seed);
  return reval;
}

void Xi::Crypto::Random::generate(ByteSpan out, ConstByteSpan seed) {
  exceptional_if<GenerationError>(
      xi_crypto_random_bytes_determenistic(out.data(), out.size(), seed.data(), seed.size()) != XI_RETURN_CODE_SUCCESS);
}
