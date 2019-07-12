// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <set>
#include <unordered_set>

#include <crypto/crypto.h>

#include "CryptoNoteCore/CryptoNote.h"
#include "CryptoNoteCore/Transactions/CachedTransaction.h"

namespace CryptoNote {

struct TransactionValidatorState {
  Crypto::KeyImageSet spentKeyImages;
};

void mergeStates(TransactionValidatorState& destination, const TransactionValidatorState& source);
bool hasIntersections(const TransactionValidatorState& destination, const TransactionValidatorState& source);

}  // namespace CryptoNote
