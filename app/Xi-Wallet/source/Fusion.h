// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <Wallet/WalletGreen.h>

bool fusionTX(CryptoNote::WalletGreen &wallet, CryptoNote::TransactionParameters p, CryptoNote::BlockHeight height);

bool optimize(CryptoNote::WalletGreen &wallet, uint64_t threshold, CryptoNote::BlockHeight height);

void fullOptimize(CryptoNote::WalletGreen &wallet, CryptoNote::BlockHeight height);

size_t makeFusionTransaction(CryptoNote::WalletGreen &wallet, uint64_t threshold, CryptoNote::BlockHeight height);
