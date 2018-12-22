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

#include "HashBasedBenchmark.h"

#include <memory>
#include <climits>
#include <random>

namespace {
using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, uint16_t>;
}

HashBasedBenchmark::HashBasedBenchmark() : BlockSize{76}, BlockCount{10} { /* */ }

const unsigned char *HashBasedBenchmark::data() const {
  static std::unique_ptr<std::vector<uint16_t>> __Data;
  if (__Data.get() == nullptr) {
    __Data = std::make_unique<std::vector<uint16_t>>();
    random_bytes_engine rbe;
    __Data->resize(BlockCount * BlockSize / 2);
    std::generate(__Data->begin(), __Data->end(), std::ref(rbe));
  }
  return reinterpret_cast<unsigned char *>(__Data->data());
}
