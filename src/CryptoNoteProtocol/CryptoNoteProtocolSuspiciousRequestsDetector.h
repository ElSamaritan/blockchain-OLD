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

#pragma once

#include <cinttypes>
#include <deque>
#include <unordered_map>

#include <Xi/ExternalIncludePush.h>
#include <boost/uuid/uuid.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Logging/LoggerRef.h>

#include "CryptoNoteProtocol/Commands/Commands.h"
#include "P2p/ConnectionContext.h"

namespace CryptoNote {
class CryptoNoteProtocolSuspiciousRequestsDetector final {
 public:
  CryptoNoteProtocolSuspiciousRequestsDetector(Logging::ILogger& logger);

  bool pushAndInspect(CryptoNoteConnectionContext& ctx, const NOTIFY_NEW_TRANSACTIONS_request& request);
  bool pushAndInspect(CryptoNoteConnectionContext& ctx, const NOTIFY_REQUEST_GET_OBJECTS_request& request,
                      const NOTIFY_RESPONSE_GET_OBJECTS_request& response);
  bool pushAndInspect(CryptoNoteConnectionContext& ctx, const NOTIFY_REQUEST_CHAIN::request& request,
                      const NOTIFY_RESPONSE_CHAIN_ENTRY_request& response);
  bool pushAndInspect(CryptoNoteConnectionContext& ctx, const NOTIFY_REQUEST_TX_POOL_request& request);
  bool pushAndInspect(CryptoNoteConnectionContext& ctx, const NOTIFY_NEW_LITE_BLOCK_request& request);
  bool pushAndInspect(CryptoNoteConnectionContext& ctx, const NOTIFY_MISSING_TXS_REQUEST& request,
                      const NOTIFY_MISSING_TXS_RESPONSE& response);

 private:
  Logging::LoggerRef m_logger;
};
}  // namespace CryptoNote
