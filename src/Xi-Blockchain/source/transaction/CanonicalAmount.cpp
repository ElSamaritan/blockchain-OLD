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

#include "Xi/Blockchain/Transaction/CanonicalAmount.hpp"

#include <cassert>
#include <limits>

#include <Xi/Algorithm/Math.h>
#include <CryptoNoteCore/CryptoNoteTools.h>

namespace Xi {
namespace Blockchain {
namespace Transaction {

// clang-format off
const Byte IndexMask  = 0b11110000;
const Byte DigitMask  = 0b00001111;
const Byte IndexShift = 4;
const Byte DigitShift = 0;
// clang-format on

namespace {

[[nodiscard]] bool decode(Byte byte, uint64_t &value) {
  XI_RETURN_EC_IF(byte == 0, false);
  uint8_t index = ((byte & IndexMask) >> IndexShift);
  uint8_t digit = ((byte & DigitMask) >> DigitShift);
  XI_RETURN_EC_IF(digit == 0, false);
  XI_RETURN_EC_IF(digit > 9, false);
  value = static_cast<uint64_t>(digit) * pow64(10ULL, index);
  XI_RETURN_SC(true);
}

[[nodiscard]] bool encode(uint64_t value, Byte &byte) {
  assert(CryptoNote::isCanonicalAmount(value));
  const auto index = CryptoNote::getCanoncialAmountDecimalPlace(value);
  XI_RETURN_EC_IF_NOT(index < 16, false);
  const auto digit = CryptoNote::getCanoncialAmountDigit(value);
  XI_RETURN_EC_IF_NOT(digit > 0, false);
  XI_RETURN_EC_IF_NOT(digit < 10, false);

  byte = 0;
  byte |= ((index << IndexShift) & IndexMask);
  byte |= ((digit << DigitShift) & DigitMask);

  XI_RETURN_SC(true);
}
}  // namespace

Xi::Blockchain::Transaction::CanonicalAmount::operator Amount() const {
  return native();
}

bool serialize(CanonicalAmount &value, Common::StringView name, CryptoNote::ISerializer &serializer) {
  if (serializer.isOutput()) {
    XI_RETURN_EC_IF_NOT(CryptoNote::isCanonicalAmount(value.native()), false);
  }

  if (serializer.isHumanReadable()) {
    XI_RETURN_EC_IF_NOT(serializer(value.value, name), false);
  } else if (serializer.isMachinery()) {
    Byte encoded = 0;
    if (serializer.isOutput()) {
      XI_RETURN_EC_IF_NOT(encode(value.native(), encoded), false);
    }

    XI_RETURN_EC_IF_NOT(serializer.binary(std::addressof(encoded), 1, name), false);

    if (serializer.isInput()) {
      XI_RETURN_EC_IF_NOT(decode(encoded, value.value), false);
    }
  } else {
    XI_RETURN_EC(false);
  }

  if (serializer.isInput()) {
    XI_RETURN_EC_IF_NOT(CryptoNote::isCanonicalAmount(value.native()), false);
  }
  XI_RETURN_SC(true);
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
