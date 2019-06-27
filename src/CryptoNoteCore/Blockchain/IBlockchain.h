/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
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

#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/IBlockchainCache.h"
#include "CryptoNoteCore/IUpgradeManager.h"
#include "CryptoNoteCore/Blockchain/IBlockchainObserver.h"
#include "CryptoNoteCore/Time/ITimeProvider.h"

namespace CryptoNote {
/*!
 * \brief The IBlockchain class encapsulates the configuration and state of a blockchain
 *
 * A blockchain may contain multiple chains, currently the framework has only the core class managing everything, this
 * interface is a future placeholder to pull some responsibilities of the core class out into smaller classes.
 */
class IBlockchain {
 public:
  virtual ~IBlockchain() = default;

  virtual void addObserver(IBlockchainObserver* observer) = 0;
  virtual void removeObserver(IBlockchainObserver* observer) = 0;

  /*!
   * \brief currency The currency configuration the blockchain is using.
   */
  virtual const Currency& currency() const = 0;

  /*!
   * \brief mainChain the current blockchain leaf considered as main chain.
   */
  virtual const IBlockchainCache* mainChain() const = 0;

  /*!
   * \brief upgradeManager The manager solving forks upgrading to a new block version
   */
  virtual const IUpgradeManager& upgradeManager() const = 0;

  /*!
   * \brief timeProvider The provider for posix timestamps used by the blockchain
   */
  virtual const ITimeProvider& timeProvider() const = 0;

  /*!
   * \brief isInitialized True if the blockchain is initialized and ready for processing queries, otherwise false.
   */
  virtual bool isInitialized() const = 0;
};

}  // namespace CryptoNote
