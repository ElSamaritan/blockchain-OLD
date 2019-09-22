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

#include <map>
#include <string>
#include <vector>
#include <cinttypes>
#include <iostream>

#include <Logging/LoggerRef.h>
#include <Xi/Crypto/FastHash.hpp>

#include "CryptoNoteBasicImpl.h"

namespace CryptoNote {
struct CheckpointData {
  uint32_t index;
  std::string blockId;
};

class Checkpoints {
 public:
  Checkpoints(Logging::ILogger& log);

  bool isEnabled() const;
  void setEnabled(bool useCheckpoints);

  [[nodiscard]] bool addCheckpoint(uint32_t index, const std::string& hash_str);
  [[nodiscard]] bool addCheckpoint(uint32_t index, const Crypto::Hash& hash);
  [[nodiscard]] bool loadCheckpoints(std::istream& stream);
  [[nodiscard]] bool loadCheckpoints(const std::string& path);
  [[nodiscard]] bool isInCheckpointZone(uint32_t index) const;
  [[nodiscard]] bool checkBlock(uint32_t index, const Crypto::Hash& h) const;
  [[nodiscard]] bool checkBlock(uint32_t index, const Crypto::Hash& h, bool& isCheckpoint) const;
  [[nodiscard]] bool isAlternativeBlockAllowed(uint32_t blockchainSize, uint32_t blockIndex) const;
  std::vector<uint32_t> getCheckpointHeights() const;
  std::size_t size() const;
  uint32_t topCheckpointIndex() const;

 private:
  bool m_enabled = false;
  std::map<uint32_t, Crypto::Hash> points;
  Logging::LoggerRef logger;
};
}  // namespace CryptoNote
