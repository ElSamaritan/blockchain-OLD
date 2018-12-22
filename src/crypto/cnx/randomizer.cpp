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

#include "crypto/cnx/randomizer.h"

#include <cmath>

Crypto::CNX::Randomizer::Randomizer(uint32_t maxSize) {
  Handle.size = 0;

  _Operators.resize(maxSize);
  Handle.operators = &_Operators[0];

  _Indices.resize(maxSize);
  Handle.indices = &_Indices[0];

  _Values.resize(maxSize);
  Handle.values = &_Values[0];
}

Crypto::CNX::Randomizer::~Randomizer() { /* */
}

void Crypto::CNX::Randomizer::reset(uint32_t size) {
  Handle.size = size;
  Handle.operationsIndex = 0;
  std::memset(_Operators.data(), 0, _Operators.size() * sizeof(uint8_t));
  std::memset(_Indices.data(), 0, _Indices.size() * sizeof(uint32_t));
  std::memset(_Values.data(), 0, _Values.size() * sizeof(int8_t));
}

uint32_t Crypto::CNX::Randomizer::size() const { return Handle.size; }
