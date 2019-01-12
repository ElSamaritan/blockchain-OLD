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

#include <crypto/CryptoTypes.h>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include "CryptoNoteCore/Transactions/CachedTransaction.h"

namespace CryptoNote {

struct PendingTransactionInfo {
  uint64_t receiveTime;
  CachedTransaction cachedTransaction;
  boost::optional<Crypto::Hash> paymentId;

  const Crypto::Hash& getTransactionHash() const;
};

}  // namespace CryptoNote
