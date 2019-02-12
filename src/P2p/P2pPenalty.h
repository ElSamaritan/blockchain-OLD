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
#include <string>

namespace CryptoNote {
/*!
 * \brief The P2pPenalty enum encodes a reason to apply penalties, the weighting is give by p2pPenaltyWeight.
 */
enum struct P2pPenalty {
  None,                          ///< No panalty should be applied.
  WrongNetworkId,                ///< The peer connected with a wrong network id.
  IncomeViolation,               ///< The peer sent payload while receiving.
  DeprecatedP2pVersion,          ///< The peers has an incompatible version.
  HandshakeFailure,              ///< The handshake failed for various reasons.
  InvalidResponse,               ///< The peer returned an invalid response. For more information view the log.
  InvalidRequest,                ///< The peer sent an invalid request. For more information view the log.
  BlockValidationFailure,        ///< The peer sent an invalid block.
  TransactionValidationFailure,  ///< The peer sent an invalid transaction.
  NoResponse,                    ///< The peer was expected to respond to a request but did not.
  Exceptional,                   ///< The request of a peer lead to an exception.
};

/*!
 * \brief p2pPenaltyWeight applies the amount of penality to apply for a given failure.
 * \param penality The failure the peer caused.
 * \return An amount of penalty to apply to the penalty score.
 */
inline uint64_t p2pPenaltyWeight(P2pPenalty penalty) {
  switch (penalty) {
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

/*!
 * \brief p2pPeanaltyMessage turns a penalty encoding into a readable string
 * \param penalty the penalty encoding to translate
 * \return a readable representation of the penalty
 */
std::string p2pPeanaltyMessage(P2pPenalty penalty) {
  switch (penalty) {
    case P2pPenalty::WrongNetworkId:
      return "wrong network identifier";
    case P2pPenalty::IncomeViolation:
      return "incoming payload violation";
    case P2pPenalty::DeprecatedP2pVersion:
      return "incompatible p2p protocol version";
    case P2pPenalty::HandshakeFailure:
      return "handshake failed";
    case P2pPenalty::InvalidResponse:
      return "returned invalid response";
    case P2pPenalty::InvalidRequest:
      return "sent invalid request";
    case P2pPenalty::BlockValidationFailure:
      return "sent invalid block";
    case P2pPenalty::TransactionValidationFailure:
      return "sent invalid transaction";
    case P2pPenalty::NoResponse:
      return "did not respond";
    case P2pPenalty::Exceptional:
      return "sent a command leading to an exception";

    case P2pPenalty::None:
      return "none";
  }
  return "unknwon";
}
}  // namespace CryptoNote
