/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include "CryptoNoteCore/IUpgradeDetector.h"

namespace CryptoNote {

class NoVotingUpgradeDetector : public IUpgradeDetector {
 public:
  NoVotingUpgradeDetector(uint8_t targetVersion, uint32_t upgradeIndex);
  ~NoVotingUpgradeDetector() override;

  uint8_t targetVersion() const override;
  uint32_t upgradeIndex() const override;

 private:
  uint8_t m_targetVersion;
  uint32_t m_upgradeIndex;
};

/*!
 * \brief makeUpgradeDetector creates a new upgrade detector for the target version and height
 * \param targetVersion The major block version to upgrade to
 * \param upgradeIndex The height of the blockchain when the new major version should be applied
 * \return An upgrade detector managing the upgrade by a version dependent strategy voting/no voting
 */
std::unique_ptr<IUpgradeDetector> makeUpgradeDetector(uint8_t targetVersion, uint32_t upgradeIndex);

}  // namespace CryptoNote
