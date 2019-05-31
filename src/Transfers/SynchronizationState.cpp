// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "SynchronizationState.h"

#include <Xi/Global.hh>


#include "Common/StdInputStream.h"
#include "Common/StdOutputStream.h"
#include "Serialization/BinaryInputStreamSerializer.h"
#include "Serialization/BinaryOutputStreamSerializer.h"
#include "CryptoNoteCore/CryptoNoteSerialization.h"

using namespace Common;

namespace CryptoNote {

SynchronizationState::ShortHistory SynchronizationState::getShortHistory(BlockHeight localHeight) const {
  ShortHistory history;
  uint32_t i = 0;
  uint32_t current_multiplier = 1;
  uint32_t sz = std::min(static_cast<uint32_t>(m_blockchain.size()), localHeight.native());

  if (!sz) return history;

  uint32_t current_back_offset = 1;
  bool genesis_included = false;

  while (current_back_offset < sz) {
    history.push_back(m_blockchain[sz - current_back_offset]);
    if (sz - current_back_offset == 0) genesis_included = true;
    if (i < 10) {
      ++current_back_offset;
    } else {
      current_back_offset += current_multiplier *= 2;
    }
    ++i;
  }

  if (!genesis_included) {
    history.push_back(m_blockchain[0]);
  }

  return history;
}

SynchronizationState::CheckResult SynchronizationState::checkInterval(const BlockchainInterval& interval) const {
  assert(interval.startHeight.native() <= m_blockchain.size());

  CheckResult result = {false, BlockHeight::Genesis, false, BlockHeight::Genesis};

  BlockHeight intervalEnd =
      interval.startHeight + BlockOffset::fromNative(static_cast<uint32_t>(interval.blocks.size()));
  BlockHeight iterationEnd =
      std::min(BlockHeight::fromNative(static_cast<BlockHeight::value_type>(m_blockchain.size())), intervalEnd);

  for (BlockHeight i = interval.startHeight; i < iterationEnd; i.advance(1)) {
    if (m_blockchain[i.native()] != interval.blocks[i.native() - interval.startHeight.native()]) {
      result.detachRequired = true;
      result.detachHeight = i;
      break;
    }
  }

  if (result.detachRequired) {
    result.hasNewBlocks = true;
    result.newBlockHeight = result.detachHeight;
    return result;
  }

  if (intervalEnd.toSize() > m_blockchain.size()) {
    result.hasNewBlocks = true;
    result.newBlockHeight = BlockHeight::fromNative(static_cast<uint32_t>(m_blockchain.size()));
  }

  return result;
}

void SynchronizationState::detach(BlockHeight height) {
  assert(height.native() < m_blockchain.size());
  m_blockchain.resize(height.native());
}

void SynchronizationState::addBlocks(const Crypto::Hash* blockHashes, BlockHeight height, uint32_t count) {
  assert(blockHashes);
  auto size = m_blockchain.size();
  if (size) {
  }
  // Dummy fix for simplewallet or walletd when sync
  if (height.isNull()) height = BlockHeight::fromIndex(0);
  assert(size == height.native());
  m_blockchain.insert(m_blockchain.end(), blockHashes, blockHashes + count);
}

BlockHeight SynchronizationState::getHeight() const {
  return BlockHeight::fromNative(static_cast<BlockHeight::value_type>(m_blockchain.size()));
}

const std::vector<Crypto::Hash>& SynchronizationState::getKnownBlockHashes() const { return m_blockchain; }

bool SynchronizationState::save(std::ostream& os) {
  StdOutputStream stream(os);
  CryptoNote::BinaryOutputStreamSerializer s(stream);
  XI_RETURN_EC_IF_NOT(serialize(s, "state"), false);
  return !os.bad();
}

bool SynchronizationState::load(std::istream& in) {
  StdInputStream stream(in);
  CryptoNote::BinaryInputStreamSerializer s(stream);
  XI_RETURN_EC_IF_NOT(serialize(s, "state"), false);
  return !in.bad();
}

bool SynchronizationState::serialize(CryptoNote::ISerializer& s, const std::string& name) {
  XI_RETURN_EC_IF_NOT(s.beginObject(name), false);
  XI_RETURN_EC_IF_NOT(s(m_blockchain, "blockchain"), false);
  s.endObject();
  return true;
}

}  // namespace CryptoNote
