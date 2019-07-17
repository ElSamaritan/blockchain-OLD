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

#include "CryptoNoteProtocol/CryptoNoteProtocolSuspiciousRequestsDetector.h"

#include <cassert>

#define P2P_CLEAR_AND_REPORT() \
  ctx.m_history.clear();       \
  return true

namespace {
struct ChainRequest {
  std::unordered_set<::Crypto::Hash> RevealedBlocks;
};
}  // namespace

CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::CryptoNoteProtocolSuspiciousRequestsDetector(
    Logging::ILogger &logger)
    : m_logger{logger, "P2pSuspicious"} {
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_NEW_TRANSACTIONS_request &request) {
  XI_UNUSED(ctx, request);
  return false;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_REQUEST_GET_OBJECTS_request &request,
    const NOTIFY_RESPONSE_GET_OBJECTS_request &response) {
  XI_UNUSED(response);
  const auto chainTimeline = ctx.m_history.getTimeline<ChainRequest>();
  if (chainTimeline.empty()) {
    m_logger(Logging::Debugging) << ctx << " requested chain objects before requesting the chain";
    P2P_CLEAR_AND_REPORT();
  }
  if (std::any_of(request.blocks.begin(), request.blocks.end(), [&chainTimeline](const auto &blockId) {
        return std::none_of(chainTimeline.begin(), chainTimeline.end(), [&blockId](const auto &occ) {
          return occ->value.RevealedBlocks.find(blockId) != occ->value.RevealedBlocks.end();
        });
      })) {
    m_logger(Logging::Debugging) << ctx << " requested a chain object he could not know about (missing chain request)";
    P2P_CLEAR_AND_REPORT();
  }
  return false;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_REQUEST_CHAIN::request &request,
    const CryptoNote::NOTIFY_RESPONSE_CHAIN_ENTRY_request &response) {
  XI_UNUSED(request);

  struct range_t {
    BlockHeight start;
    uint32_t count;
  };

  ctx.m_history.pushOccurrence<range_t>(
      range_t{response.start_height, static_cast<uint32_t>(response.block_hashes.size())}, 2);
  {
    ChainRequest revealedBlocks{{response.block_hashes.begin(), response.block_hashes.end()}};
    ctx.m_history.pushOccurrence(revealedBlocks, 1);
  }
  const auto timeline = ctx.m_history.getTimeline<range_t>();
  assert(timeline.size() > 0);
  if (timeline.size() > 1) {
    for (size_t i = 1; i < timeline.size(); ++i) {
      if (timeline[i]->timestamp - timeline[i - 1]->timestamp > std::chrono::minutes{1}) {
        continue;
      }
      if (timeline[i - 1]->value.start + BlockOffset::fromNative(timeline[i - 1]->value.count) >
          timeline[i]->value.start + BlockOffset::fromNative(1)) {
        m_logger(Logging::Debugging) << ctx << " none consecutive chain request, last_end="
                                     << timeline[i - 1]->value.start.native() + timeline[i - 1]->value.count
                                     << " current_start=" << timeline[i]->value.start.native();
        P2P_CLEAR_AND_REPORT();
      }
    }
  }

  return false;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_REQUEST_TX_POOL_request &request) {
  XI_UNUSED(ctx, request);
  return false;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_NEW_LITE_BLOCK_request &request) {
  XI_UNUSED(ctx, request);
  return false;
}

bool CryptoNote::CryptoNoteProtocolSuspiciousRequestsDetector::pushAndInspect(
    CryptoNote::CryptoNoteConnectionContext &ctx, const CryptoNote::NOTIFY_MISSING_TXS_REQUEST &request,
    const NOTIFY_MISSING_TXS_RESPONSE &response) {
  XI_UNUSED(ctx, request, response);
  return false;
}
