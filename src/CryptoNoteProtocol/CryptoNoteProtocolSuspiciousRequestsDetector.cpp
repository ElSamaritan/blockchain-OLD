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

#include "CryptoNoteProtocol/CryptoNoteProtocolSuspiciousRequestsDetector.h"

#include <cassert>

CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::CryptoNoteProtocolSuspiciousRequestsDetector(
    Logging::ILogger &logger)
    : m_logger{logger, "P2pSuspicious"} {}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_NEW_TRANSACTIONS_request &request) {
  ctx.m_history.pushOccurrence(request);
  return true;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_REQUEST_GET_OBJECTS_request &request) {
  ctx.m_history.pushOccurrence(request);
  return true;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const NOTIFY_REQUEST_CHAIN::request &request) {
  ctx.m_history.pushOccurrence(request);
  return true;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_REQUEST_TX_POOL_request &request) {
  ctx.m_history.pushOccurrence(request);
  return true;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_NEW_LITE_BLOCK_request &request) {
  ctx.m_history.pushOccurrence(request);
  return true;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_MISSING_TXS_REQUEST &request) {
  ctx.m_history.pushOccurrence(request);
  return true;
}
