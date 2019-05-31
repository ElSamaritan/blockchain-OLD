// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "CryptoNoteCore/Transactions/TransactionValidatiorState.h"

namespace CryptoNote {

void mergeStates(TransactionValidatorState& destination, const TransactionValidatorState& source) {
  destination.spentKeyImages.insert(source.spentKeyImages.begin(), source.spentKeyImages.end());
}

bool hasIntersections(const TransactionValidatorState& destination, const TransactionValidatorState& source) {
  return std::any_of(source.spentKeyImages.begin(), source.spentKeyImages.end(),
                     [&](const Crypto::KeyImage& ki) { return destination.spentKeyImages.count(ki) != 0; });
}

}  // namespace CryptoNote
