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

#include "CryptoNoteCore/Transactions/TransactionValidationContext.h"

#include <exception>
#include <limits>

#include "CryptoNoteCore/IBlockchainCache.h"

CryptoNote::TransactionValidationContext::TransactionValidationContext(const IBlockchainCache &blockchain,
                                                                       const IUpgradeManager &upgradeManager,
                                                                       const Currency &currency,
                                                                       const Checkpoints &checkpoints)
    : m_blockchain{blockchain},
      m_upgradeManager{upgradeManager},
      m_currency{currency},
      m_checkpoints{checkpoints},
      m_block{} {}

CryptoNote::TransactionValidationContext::TransactionValidationContext(const IBlockchainCache &blockchain,
                                                                       const IUpgradeManager &upgradeManager,
                                                                       const Currency &currency,
                                                                       const Checkpoints &checkpoints,
                                                                       const BlockHeader &header, uint32_t height)
    : m_blockchain{blockchain},
      m_upgradeManager{upgradeManager},
      m_currency{currency},
      m_checkpoints{checkpoints},
      m_block{BlockInfo{header, height}} {}

bool CryptoNote::TransactionValidationContext::isContainedInBlock() const { return m_block.is_initialized(); }

bool CryptoNote::TransactionValidationContext::isContainedInCheckpointsRange() const {
  return isContainedInBlock() && m_checkpoints.isInCheckpointZone(m_block->Height);
}

CryptoNote::BlockInfo CryptoNote::TransactionValidationContext::containingBlock() const {
  if (!isContainedInBlock())
    throw std::runtime_error{
        "The TransactionValidationContext is not associated to a block. Check the context with isContainedInBlock "
        "before quering the block info."};
  return m_block.get();
}

uint32_t CryptoNote::TransactionValidationContext::validationHeight() const {
  if (isContainedInBlock()) {
    return containingBlock().Height - 1;
  } else {
    return blockchain().getTopBlockIndex() + 1;
  }
}

uint8_t CryptoNote::TransactionValidationContext::validationMajorBlockVersion() const {
  if (isContainedInBlock()) {
    return containingBlock().Header.majorVersion;
  } else {
    return upgradeManager().getBlockMajorVersion(blockchain().getTopBlockIndex() + 1);
  }
}

const CryptoNote::IBlockchainCache &CryptoNote::TransactionValidationContext::blockchain() const {
  return m_blockchain;
}

const CryptoNote::IUpgradeManager &CryptoNote::TransactionValidationContext::upgradeManager() const {
  return m_upgradeManager;
}

const CryptoNote::Currency &CryptoNote::TransactionValidationContext::currency() const { return m_currency; }

const Crypto::KeyImagesSet &CryptoNote::TransactionValidationContext::keyImages() const { return m_keyImages; }
