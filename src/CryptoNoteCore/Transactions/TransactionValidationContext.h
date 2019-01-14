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

#pragma once

#include <cinttypes>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Global.h>

#include <crypto/CryptoTypes.h>

#include "CryptoNoteCore/Checkpoints.h"
#include "CryptoNoteCore/IUpgradeManager.h"
#include "CryptoNoteCore/IBlockchainCache.h"
#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/BlockInfo.h"

namespace CryptoNote {
class IBlockchainCache;

class TransactionValidationContext {
 public:
  explicit TransactionValidationContext(const IBlockchainCache& blockchain, const IUpgradeManager& upgradeManager,
                                        const Currency& currency, const Checkpoints& checkpoints);
  explicit TransactionValidationContext(const IBlockchainCache& blockchain, const IUpgradeManager& upgradeManager,
                                        const Currency& currency, const Checkpoints& checkpoints,
                                        const BlockHeader& header, uint32_t height);
  ~TransactionValidationContext();

  const IBlockchainCache& blockchain() const;
  const IUpgradeManager& upgradeManager() const;
  const Currency& currency() const;

  const Crypto::KeyImagesSet& keyImages() const;

  bool isContainedInBlock() const;
  bool isContainedInCheckpointsRange() const;
  BlockInfo containingBlock() const;

  uint32_t validationHeight() const;
  uint8_t validationMajorBlockVersion() const;

 private:
  const IBlockchainCache& m_blockchain;
  const IUpgradeManager& m_upgradeManager;
  const Currency& m_currency;
  const Checkpoints& m_checkpoints;
  boost::optional<BlockInfo> m_block;
  Crypto::KeyImagesSet m_keyImages;
};
}  // namespace CryptoNote
