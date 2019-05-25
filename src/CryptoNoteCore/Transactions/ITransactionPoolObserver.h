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

namespace CryptoNote {
class ITransactionPoolObserver {
 public:
  enum struct AdditionReason {
    Incoming,         ///< The transaction came in without any related block, thus a user pushed the transaction to the
                      ///< blockchain.
    MainChainSwitch,  ///< The blockchain switched to a main chain. Valid transactions that are not contained by the new
                      ///< main chain gets readded.
    Deserialization,  ///< The transaction was restored from an archive or other serialization interface. Commonly this
                      ///< happens when the persistent transaction pool storage is loaded.
    SkipNotification,  ///< Skips the notification of observers.
  };

  enum struct DeletionReason {
    AddedToMainChain,          ///< A block was added to the main chain containing the transaction.
    KeyImageUsedInMainChain,   ///< A block was added to the main chain that uses the same input as one of the
                               ///< transactions in the pool. The pool transaction is now invalid as it would perform
                               ///< double spending once added to the main chain.
    BlockMajorVersionUpgrade,  ///< The major version in the blockchain upgraded introducing new constraints the
                               ///< transaction does not satisfy anymore
    PoolCleanupProcedure,      ///< Transaction was in pool but got outdated or invalid while remaining there.
    Forced,                    ///< The user forced the deletion of the transaction
    SkipNotification,          ///< Skips the notification of observers.
  };

 public:
  virtual ~ITransactionPoolObserver() = default;

  // TODO make batched variants
  virtual void transactionDeletedFromPool(const Crypto::Hash& hash, DeletionReason reason) = 0;
  virtual void transactionAddedToPool(const Crypto::Hash& hash, AdditionReason reason) = 0;
};
}  // namespace CryptoNote
