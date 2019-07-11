﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The Calex Developers
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

#include "Checkpoints.h"
#include "Common/StringTools.h"
#include <fstream>

using namespace Logging;

namespace CryptoNote {
//---------------------------------------------------------------------------
Checkpoints::Checkpoints(Logging::ILogger &log) : logger(log, "checkpoints") {}

bool Checkpoints::isEnabled() const { return m_enabled; }

void Checkpoints::setEnabled(bool useCheckpoints) { m_enabled = useCheckpoints; }
//---------------------------------------------------------------------------
bool Checkpoints::addCheckpoint(uint32_t index, const std::string &hash_str) {
  auto hashParseResult = Crypto::Hash::fromString(hash_str);

  if (hashParseResult.isError()) {
    logger(Error) << "INVALID HASH IN CHECKPOINTS!";
    return false;
  } else {
    return addCheckpoint(index, hashParseResult.value());
  }
}

bool Checkpoints::addCheckpoint(uint32_t index, const Crypto::Hash &hash) {
  /* The return value lets us check if it was inserted or not. If it wasn't,
     there is already a key (i.e., a height value) existing */
  if (!points.insert({index, hash}).second) {
    logger(Error) << "CHECKPOINT ALREADY EXISTS!";
    return false;
  }
  return true;
}

bool Checkpoints::loadCheckpointsFromFile(const std::string &filename) {
  std::ifstream file(filename);

  if (!file) {
    logger(Error) << "Could not load checkpoints file: " << filename;

    return false;
  }

  /* The block this checkpoint is for (as a string) */
  std::string indexString;

  /* The hash this block has (as a string) */
  std::string hash;

  /* The block index (as a uint32_t) */
  uint32_t index;

  /* Checkpoints file has this format:

     index,hash
     index2,hash2

     So, we do std::getline() on the file with the delimiter as ',' to take
     the index, then we do std::getline() on the file again with the standard
     delimiter of '\n', to get the hash. */
  while (std::getline(file, indexString, ','), std::getline(file, hash)) {
    /* Try and parse the indexString as an int */
    try {
      index = std::stoi(indexString);
    } catch (const std::invalid_argument &) {
      logger(Error) << "Invalid checkpoint file format - "
                    << "could not parse height as a number";

      return false;
    }

    /* Failed to parse hash, or checkpoint already exists */
    if (!addCheckpoint(index, hash)) {
      return false;
    }
  }

  logger(Info) << "Loaded " << points.size() << " checkpoints from " << filename;

  return true;
}

//---------------------------------------------------------------------------
bool Checkpoints::isInCheckpointZone(uint32_t index) const {
  return m_enabled && !points.empty() && (index <= (--points.end())->first);
}
//---------------------------------------------------------------------------
bool Checkpoints::checkBlock(uint32_t index, const Crypto::Hash &h, bool &isCheckpoint) const {
  if (!m_enabled) {
    return false;
  }
  auto it = points.find(index);
  isCheckpoint = it != points.end();
  if (!isCheckpoint) return true;

  if (it->second == h) {
    if (index % 100 == 0) {
      logger(Logging::Info, CYAN) << "CHECKPOINT PASSED FOR INDEX " << index << " " << h;
    }
    return true;
  } else {
    logger(Logging::Warning) << "CHECKPOINT FAILED FOR HEIGHT " << index << ". EXPECTED HASH: " << it->second
                             << ", FETCHED HASH: " << h;
    return false;
  }
}
//---------------------------------------------------------------------------
bool Checkpoints::checkBlock(uint32_t index, const Crypto::Hash &h) const {
  bool ignored;
  return checkBlock(index, h, ignored);
}
//---------------------------------------------------------------------------
bool Checkpoints::isAlternativeBlockAllowed(uint32_t blockchainSize, uint32_t blockIndex) const {
  if (blockchainSize == 0) {
    return false;
  }

  auto it = points.upper_bound(blockchainSize);
  // Is blockchainSize before the first checkpoint?
  if (it == points.begin()) {
    return true;
  }

  --it;
  uint32_t checkpointIndex = it->first;
  return checkpointIndex < blockIndex;
}

std::vector<uint32_t> Checkpoints::getCheckpointHeights() const {
  std::vector<uint32_t> checkpointHeights;
  checkpointHeights.reserve(points.size());
  for (const auto &it : points) {
    checkpointHeights.push_back(it.first);
  }

  return checkpointHeights;
}

std::size_t Checkpoints::size() const { return points.size(); }

uint32_t Checkpoints::topCheckpointIndex() const {
  if (points.empty()) {
    return 0;
  } else {
    return points.rbegin()->first;
  }
}

}  // namespace CryptoNote
