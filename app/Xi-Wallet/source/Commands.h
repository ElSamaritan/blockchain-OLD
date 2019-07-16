// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <string>
#include <vector>
#include <Xi/Config/WalletConfig.h>
#include <CryptoNoteCore/Currency.h>

class Command {
 public:
  Command(std::string commandName, std::string description) : commandName(commandName), description(description) {}

  const std::string commandName;
  const std::string description;
};

class AdvancedCommand : public Command {
 public:
  AdvancedCommand(std::string commandName, std::string description, bool viewWalletSupport, bool advanced)
      : Command(commandName, description), viewWalletSupport(viewWalletSupport), advanced(advanced) {}

  const bool viewWalletSupport;
  const bool advanced;
};

std::vector<Command> startupCommands();

std::vector<Command> nodeDownCommands();

std::vector<AdvancedCommand> allCommands(const CryptoNote::Currency& currency);

std::vector<AdvancedCommand> basicCommands(const CryptoNote::Currency& currency);

std::vector<AdvancedCommand> advancedCommands(const CryptoNote::Currency& currency);

std::vector<AdvancedCommand> basicViewWalletCommands(const CryptoNote::Currency& currency);

std::vector<AdvancedCommand> advancedViewWalletCommands(const CryptoNote::Currency& currency);

std::vector<AdvancedCommand> allViewWalletCommands(const CryptoNote::Currency& currency);
