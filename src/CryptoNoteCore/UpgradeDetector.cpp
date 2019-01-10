/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include "UpgradeDetector.h"

CryptoNote::NoVotingUpgradeDetector::NoVotingUpgradeDetector(uint8_t targetVersion, uint32_t upgradeIndex)
    : m_targetVersion(targetVersion), m_upgradeIndex(upgradeIndex) {}

CryptoNote::NoVotingUpgradeDetector::~NoVotingUpgradeDetector() {}

uint8_t CryptoNote::NoVotingUpgradeDetector::targetVersion() const { return m_targetVersion; }

uint32_t CryptoNote::NoVotingUpgradeDetector::upgradeIndex() const { return m_upgradeIndex; }

std::unique_ptr<CryptoNote::IUpgradeDetector> CryptoNote::makeUpgradeDetector(uint8_t targetVersion,
                                                                              uint32_t upgradeIndex) {
  return std::unique_ptr<NoVotingUpgradeDetector>(new NoVotingUpgradeDetector(targetVersion, upgradeIndex));
}
