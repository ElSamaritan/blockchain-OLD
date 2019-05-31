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

#include "Xi/Crypto/Random/Engine.hpp"

#include <Xi/Exceptions.hpp>

Xi::Crypto::Random::Engine32::Engine32() : m_state{xi_crypto_random_state_create(), xi_crypto_random_state_destroy} {
  exceptional_if<RuntimeError>(m_state.get() == nullptr, "random state allocation failed.");
  seed();
}

Xi::Crypto::Random::Engine32::Engine32(Xi::Crypto::Random::Engine32::seed_type _seed)
    : m_state{xi_crypto_random_state_create(), xi_crypto_random_state_destroy} {
  exceptional_if<RuntimeError>(m_state.get() == nullptr, "random state allocation failed.");
  seed(_seed);
}

Xi::Crypto::Random::Engine32::~Engine32() {}

void Xi::Crypto::Random::Engine32::seed() {
  const int ec = xi_crypto_random_state_init(m_state.get());
  exceptional_if_not<RuntimeError>(ec == XI_RETURN_CODE_SUCCESS, "random state initialization faield");
}

void Xi::Crypto::Random::Engine32::seed(Xi::Crypto::Random::Engine32::seed_type seed) {
  const int ec = xi_crypto_random_state_init_determentistic(m_state.get(), seed.data(), seed.size_bytes());
  exceptional_if_not<RuntimeError>(ec == XI_RETURN_CODE_SUCCESS, "random state initialization failed");
}

Xi::Crypto::Random::Engine32::result_type Xi::Crypto::Random::Engine32::operator()() {
  result_type reval;
  this->operator()(asByteSpan(&reval, sizeof(reval)));
  return reval;
}

void Xi::Crypto::Random::Engine32::operator()(Xi::ByteSpan out) {
  const int ec = xi_crypto_random_bytes_from_state(out.data(), out.size_bytes(), m_state.get());
  exceptional_if_not<RuntimeError>(ec == XI_RETURN_CODE_SUCCESS, "random byte generation failed");
}

Xi::Crypto::Random::Engine64::Engine64() : m_state{xi_crypto_random_state_create(), xi_crypto_random_state_destroy} {
  exceptional_if<RuntimeError>(m_state.get() == nullptr, "random state allocation failed.");
  seed();
}

Xi::Crypto::Random::Engine64::Engine64(Xi::Crypto::Random::Engine64::seed_type _seed)
    : m_state{xi_crypto_random_state_create(), xi_crypto_random_state_destroy} {
  exceptional_if<RuntimeError>(m_state.get() == nullptr, "random state allocation failed.");
  seed(_seed);
}

Xi::Crypto::Random::Engine64::~Engine64() {}

void Xi::Crypto::Random::Engine64::seed() {
  const int ec = xi_crypto_random_state_init(m_state.get());
  exceptional_if_not<RuntimeError>(ec == XI_RETURN_CODE_SUCCESS, "random state initialization faield");
}

void Xi::Crypto::Random::Engine64::seed(Xi::Crypto::Random::Engine64::seed_type seed) {
  const int ec = xi_crypto_random_state_init_determentistic(m_state.get(), seed.data(), seed.size_bytes());
  exceptional_if_not<RuntimeError>(ec == XI_RETURN_CODE_SUCCESS, "random state initialization failed");
}

Xi::Crypto::Random::Engine64::result_type Xi::Crypto::Random::Engine64::operator()() {
  result_type reval;
  this->operator()(asByteSpan(&reval, sizeof(reval)));
  return reval;
}

void Xi::Crypto::Random::Engine64::operator()(Xi::ByteSpan out) {
  const int ec = xi_crypto_random_bytes_from_state(out.data(), out.size_bytes(), m_state.get());
  exceptional_if_not<RuntimeError>(ec == XI_RETURN_CODE_SUCCESS, "random byte generation failed");
}
