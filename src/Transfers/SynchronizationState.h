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

#pragma once

#include <vector>
#include <map>
#include <string>

#include <CryptoNoteCore/Blockchain/BlockHeight.hpp>

#include "CommonTypes.h"
#include "Common/IStreamSerializable.h"
#include "Serialization/ISerializer.h"

namespace CryptoNote {

class SynchronizationState : public IStreamSerializable {
 public:
  struct CheckResult {
    bool detachRequired;
    BlockHeight detachHeight;
    bool hasNewBlocks;
    BlockHeight newBlockHeight;
  };

  typedef std::vector<Crypto::Hash> ShortHistory;

  explicit SynchronizationState(const Crypto::Hash& genesisBlockHash) { m_blockchain.push_back(genesisBlockHash); }
  ~SynchronizationState() override = default;

  ShortHistory getShortHistory(BlockHeight localHeight) const;
  CheckResult checkInterval(const BlockchainInterval& interval) const;

  void detach(BlockHeight height);
  void addBlocks(const Crypto::Hash* blockHashes, BlockHeight height, uint32_t count);
  BlockHeight getHeight() const;
  const std::vector<Crypto::Hash>& getKnownBlockHashes() const;

  // IStreamSerializable
  [[nodiscard]] virtual bool save(std::ostream& os) override;
  [[nodiscard]] virtual bool load(std::istream& in) override;

  // serialization
  [[nodiscard]] bool serialize(CryptoNote::ISerializer& s, const std::string& name);

 private:
  std::vector<Crypto::Hash> m_blockchain;
};

}  // namespace CryptoNote
