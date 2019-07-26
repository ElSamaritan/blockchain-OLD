// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <System/Dispatcher.h>
#include <Logging/ILogger.h>
#include <CryptoNoteCore/INode.h>
#include <CryptoNoteCore/Currency.h>

#include <Types.h>
#include "WalletOptions.h"

int execZedWallet(System::Dispatcher* dispatcher, CryptoNote::INode& node, XiWallet::WalletOptions& options,
                  const CryptoNote::Currency& currency, Logging::ILogger& logger);

int run(CryptoNote::WalletGreen& wallet, CryptoNote::INode& node, XiWallet::WalletOptions& config);
