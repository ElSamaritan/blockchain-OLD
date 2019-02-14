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

#include <crypto/CryptoTypes.h>

#include "CryptoNoteCore/IBlockchainCache.h"

namespace CryptoNote {
/*!
 * \brief The IBlockchainObserver class defines all notifications sent by a blockchain to inform about state changes.
 */
class IBlockchainObserver {
 public:
  virtual ~IBlockchainObserver() = default;

  virtual void blockAdded(uint32_t index, const Crypto::Hash& hash) = 0;

  /*!
   * \brief mainChainSwitched A leaf of the blockchain reached higher cumulative difficulty than the current main chain
   * \param previous The previous leaf considered as main chain
   * \param current The new leaf considered as main chain
   * \param splitIndex The first block index not shared by both blockchains, both chains contain the same blocks for
   * every block with a lower index
   */
  virtual void mainChainSwitched(const IBlockchainCache& previous, const IBlockchainCache& current,
                                 uint32_t splitIndex) = 0;
};
}  // namespace CryptoNote
