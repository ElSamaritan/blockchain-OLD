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

#include <cstdint>

#include <Xi/Blockchain/Block/Version.hpp>

namespace CryptoNote {

class IUpgradeManager {
 public:
  virtual ~IUpgradeManager() {}

  virtual void addBlockVersion(Xi::Blockchain::Block::Version targetVersion, uint32_t upgradeHeight, bool fork) = 0;
  virtual Xi::Blockchain::Block::Version getBlockVersion(uint32_t blockIndex) const = 0;
  virtual bool isFork(Xi::Blockchain::Block::Version version) const = 0;
  virtual uint32_t getForkIndex(Xi::Blockchain::Block::Version version) const = 0;
  virtual Xi::Blockchain::Block::Version maximumVersion() const = 0;
};

inline std::vector<uint32_t> getForks(const IUpgradeManager& manager) {
  std::vector<uint32_t> reval{};
  reval.reserve(manager.maximumVersion().native());
  for (Xi::Blockchain::Block::Version::value_type i = Xi::Blockchain::Block::Version::Genesis.native();
       i <= manager.maximumVersion().native(); ++i) {
    const Xi::Blockchain::Block::Version current{i};
    if (manager.isFork(current)) {
      reval.push_back(manager.getForkIndex(current));
    }
  }
  return reval;
}

}  // namespace CryptoNote
