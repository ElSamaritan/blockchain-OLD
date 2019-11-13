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

#include "RemoteExporter.h"

#include <algorithm>
#include <iterator>
#include <utility>

XiSync::RemoteExporter::RemoteExporter(CryptoNote::INode &remote, XiSync::DumpWriter &writer, Logging::ILogger &logger)
    : Exporter(writer, logger), m_remote{remote} {}

uint32_t XiSync::RemoteExporter::topBlockIndex() const {
  const auto remoteStatus = m_remote.getLastBlockHeaderInfo().get().takeOrThrow();
  return remoteStatus.height.toIndex();
}

std::vector<CryptoNote::RawBlock> XiSync::RemoteExporter::queryBlocks(uint32_t startIndex, uint32_t count) const {
  std::vector<CryptoNote::RawBlock> reval;
  reval.reserve(count);
  for (uint32_t index = startIndex; index < startIndex + count;) {
    const uint32_t left = (startIndex + count) - index;
    auto blocks = m_remote.getRawBlocksByRange(CryptoNote::BlockHeight::fromIndex(index), std::min<uint32_t>(left, 50))
                      .get()
                      .takeOrThrow();
    index += static_cast<uint32_t>(blocks.size());
    std::transform(blocks.begin(), blocks.end(), std::back_inserter(reval), [](auto &&b) { return std::move(b); });
  }
  return reval;
}
