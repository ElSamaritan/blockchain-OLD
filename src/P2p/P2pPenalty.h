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
enum struct P2pPenalty {
  None,
  WrongNetworkId,
  IncomeViolation,
  DeprecatedP2pVersion,
  HandshakeFailure,
  InvalidResponse,
  InvalidRequest,
  BlockValidationFailure,
  TransactionValidationFailure,
  NoResponse,
  Exceptional
};

inline uint64_t p2pPenalityWeight(P2pPenalty penality) {
  switch (penality) {
    case P2pPenalty::WrongNetworkId:
      return 10;
    case P2pPenalty::IncomeViolation:
      return 5;
    case P2pPenalty::DeprecatedP2pVersion:
      return 2;
    case P2pPenalty::HandshakeFailure:
      return 2;
    case P2pPenalty::InvalidResponse:
      return 4;
    case P2pPenalty::InvalidRequest:
      return 4;
    case P2pPenalty::BlockValidationFailure:
      return 5;
    case P2pPenalty::TransactionValidationFailure:
      return 5;
    case P2pPenalty::NoResponse:
      return 2;
    case P2pPenalty::Exceptional:
      return 4;

    case P2pPenalty::None:
      return 0;
  }
  return 0;
}
}  // namespace CryptoNote
