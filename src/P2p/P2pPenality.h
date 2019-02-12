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

#include <cinttypes>

namespace CryptoNote {
enum struct P2pPenality {
  None,
  WrongNetworkId,
  IncomeViolation,
  DeprecatedP2pVersion,
  HandshakeFailure,
  InvalidResponse,
  InvalidRequest,
  BlockValidationFailure
};

inline uint64_t p2pPenalityWeight(P2pPenality penality) {
  switch (penality) {
    case P2pPenality::WrongNetworkId:
      return 10;
    case P2pPenality::IncomeViolation:
      return 5;
    case P2pPenality::DeprecatedP2pVersion:
      return 2;
    case P2pPenality::HandshakeFailure:
      return 2;
    case P2pPenality::InvalidResponse:
      return 4;
    case P2pPenality::InvalidRequest:
      return 4;
    case P2pPenality::BlockValidationFailure:
      return 5;

    case P2pPenality::None:
      return 0;
  }
  return 0;
}
}  // namespace CryptoNote
