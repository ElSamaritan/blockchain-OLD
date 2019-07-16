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

#include "UpgradeManager.h"

#include <stdexcept>

#include <Xi/Config.h>
#include <Xi/Exceptions.hpp>

namespace CryptoNote {

UpgradeManager::UpgradeManager() {}

UpgradeManager::~UpgradeManager() {}

void UpgradeManager::addBlockVersion(Xi::Blockchain::Block::Version targetVersion, uint32_t upgradeHeight, bool fork) {
  assert(m_upgradeDetectors.empty() || m_upgradeDetectors.back()->targetVersion() < targetVersion);
  m_upgradeDetectors.emplace_back(makeUpgradeDetector(targetVersion, upgradeHeight, fork));
}

Xi::Blockchain::Block::Version UpgradeManager::getBlockVersion(uint32_t blockIndex) const {
  for (auto it = m_upgradeDetectors.rbegin(); it != m_upgradeDetectors.rend(); ++it) {
    if (it->get()->upgradeIndex() <= blockIndex) {
      return it->get()->targetVersion();
    }
  }

  return Xi::Blockchain::Block::Version::Genesis;
}

bool UpgradeManager::isFork(Xi::Blockchain::Block::Version version) const {
  auto search = std::find_if(m_upgradeDetectors.begin(), m_upgradeDetectors.end(),
                             [version](const auto& detector) { return detector->targetVersion() == version; });
  if (search == m_upgradeDetectors.end()) {
    return false;
  } else {
    return (*search)->isFork();
  }
}

uint32_t UpgradeManager::getForkIndex(Xi::Blockchain::Block::Version version) const {
  auto search = std::find_if(m_upgradeDetectors.begin(), m_upgradeDetectors.end(),
                             [version](const auto& detector) { return detector->targetVersion() == version; });
  if (search == m_upgradeDetectors.end()) {
    Xi::exceptional<Xi::NotFoundError>();
  } else {
    return (*search)->upgradeIndex();
  }
}

Xi::Blockchain::Block::Version UpgradeManager::maximumVersion() const {
  return Xi::Blockchain::Block::Version{
      static_cast<Xi::Blockchain::Block::Version::value_type>(m_upgradeDetectors.size())};
}

}  // namespace CryptoNote
