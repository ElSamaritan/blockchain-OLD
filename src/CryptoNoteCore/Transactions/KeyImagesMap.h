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

#include <unordered_map>
#include <unordered_set>

#include <Xi/Global.h>
#include <Xi/Concurrent/ReadersWriterLock.h>

#include <crypto/CryptoTypes.h>
#include <Logging/LoggerRef.h>

#include "CryptoNoteCore/CryptoNote.h"

namespace CryptoNote {
/*!
 * \brief The KeyImagesContainer class stores key images and their references to transactions they are contained by
 */
class KeyImagesMap {
  XI_DELETE_COPY(KeyImagesMap);
  XI_DELETE_MOVE(KeyImagesMap);

 public:
  using key_image_t = Crypto::KeyImage;
  using transaction_id_t = Crypto::Hash;

 public:
  explicit KeyImagesMap(Logging::ILogger& logger);
  ~KeyImagesMap();

  Crypto::KeyImagesSet keyImages() const;
  std::unordered_set<transaction_id_t> transactionsByKeyImage(const key_image_t& keyImage) const;

  bool contains(const key_image_t& keyImage) const;

  bool addTransactionInputs(const transaction_id_t& txId, const Transaction& tx);
  bool removeTransactionInputs(const transaction_id_t& txId, const Transaction& tx);

 private:
  Logging::LoggerRef m_logger;
  Xi::Concurrent::ReadersWriterLock m_mutationLock;
  std::unordered_map<key_image_t, std::unordered_set<transaction_id_t>> m_keyImages;
};
}  // namespace CryptoNote
