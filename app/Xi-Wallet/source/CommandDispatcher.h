﻿// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <memory>
#include <string>

#include <Types.h>

#include "WalletOptions.h"

bool handleCommand(const std::string command, std::shared_ptr<WalletInfo> walletInfo, CryptoNote::INode &node);

std::shared_ptr<WalletInfo> handleLaunchCommand(CryptoNote::WalletGreen &wallet, std::string launchCommand,
                                                XiWallet::WalletOptions &config);
