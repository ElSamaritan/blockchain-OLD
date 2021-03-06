﻿// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <tuple>
#include <string>
#include <vector>

#include <Types.h>

#include "WalletOptions.h"

template <typename T>
std::string parseCommand(const std::vector<T> &printableCommands, const std::vector<T> &availableCommands,
                         std::string prompt, bool backgroundRefresh, std::shared_ptr<WalletInfo> walletInfo);

std::tuple<bool, std::shared_ptr<WalletInfo>> selectionScreen(XiWallet::WalletOptions &config,
                                                              CryptoNote::WalletGreen &wallet, CryptoNote::INode &node);

bool checkNodeStatus(CryptoNote::INode &node);

std::string getAction(XiWallet::WalletOptions &config);

void mainLoop(std::shared_ptr<WalletInfo> walletInfo, CryptoNote::INode &node);

template <typename T>
void printCommands(const std::vector<T> &commands, int offset = 0);
