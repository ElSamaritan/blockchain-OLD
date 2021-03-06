﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "ConsoleHandler.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#else
#include <unistd.h>
#include <stdio.h>
#endif

#include <Xi/ExternalIncludePush.h>
#include <linenoise.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Algorithm/String.h>
#include <Serialization/ConsoleOutputSerializer.hpp>

using Common::Console::Color;

namespace Common {

/////////////////////////////////////////////////////////////////////////////
// ConsoleHandler
/////////////////////////////////////////////////////////////////////////////
ConsoleHandler::ConsoleHandler() {
  setHandler(
      "quit", [](const auto&) { return true; }, "Shuts down the application gracefully.");
  setHandler(
      "clear",
      [this](const auto&) {
        this->clear();
        return true;
      },
      "Clears the screen.");
}

ConsoleHandler::~ConsoleHandler() {
  /* */
}

void ConsoleHandler::run(const std::string& prompt, const std::string& datadir) {
  auto historyFile = datadir + std::string{"/"} + prompt + std::string{".history"};
  linenoise::LoadHistory(historyFile.c_str());
  linenoise::SetHistoryMaxLen(128);
  linenoise::SetCompletionCallback(
      std::bind(&ConsoleHandler::completionCallback, this, std::placeholders::_1, std::placeholders::_2));

  const auto prefix = prompt + std::string{" > "};
  std::string line{};
  while (!linenoise::Readline(prefix.c_str(), line)) {
    try {
      boost::algorithm::trim(line);

      if (line == "quit") {
        quit();
        break;
      }

      if (!line.empty()) {
        handleCommand(line);
      }

    } catch (std::exception& e) {
      printError(e.what());
    }
    line.clear();
  }

  linenoise::SetCompletionCallback([](const char*, std::vector<std::string>&) {});
  linenoise::SaveHistory(historyFile.c_str());
}

std::unique_ptr<CryptoNote::ISerializer> ConsoleHandler::makeOutputSerialiizer() const {
  return std::make_unique<CryptoNote::ConsoleOutputSerializer>(std::cout);
}

void ConsoleHandler::printError(std::string error) {
  std::cerr << error << std::endl;
}

void ConsoleHandler::printWarning(std::string warn) {
  std::cout << warn << std::endl;
}

void ConsoleHandler::printMessage(std::string msg) {
  std::cout << msg << std::endl;
}

void ConsoleHandler::doQuit() {
  /* */
}

std::string ConsoleHandler::getUsage() const {
  if (m_handlers.empty()) {
    return std::string();
  }

  std::stringstream ss;

  size_t maxlen = std::max_element(m_handlers.begin(), m_handlers.end(),
                                   [](CommandHandlersMap::const_reference& a, CommandHandlersMap::const_reference& b) {
                                     return a.first.size() < b.first.size();
                                   })
                      ->first.size();

  for (auto& x : m_handlers) {
    ss << std::left << std::setw(maxlen + 3) << x.first << x.second.second << std::endl;
  }

  return ss.str();
}

void ConsoleHandler::setHandler(const std::string& command, const ConsoleCommandHandler& handler,
                                const std::string& usage) {
  m_handlers[Xi::replace(command, "_", " ")] = std::make_pair(handler, usage);
}

bool ConsoleHandler::runCommand(const std::vector<std::string>& cmdAndArgs) {
  if (cmdAndArgs.size() == 0) {
    return false;
  }

  const auto& cmd = cmdAndArgs.front();
  auto hIter = m_handlers.find(cmd);

  if (hIter == m_handlers.end()) {
    printWarning(std::string{"Unknown command: "} + cmd);
    return false;
  }

  std::vector<std::string> args(cmdAndArgs.begin() + 1, cmdAndArgs.end());
  hIter->second.first(args);
  return true;
}

void ConsoleHandler::handleCommand(std::string cmd) {
  linenoise::AddHistory(cmd.c_str());
  std::string bestMatch{};
  for (const auto& handler : m_handlers) {
    if (Xi::starts_with(cmd, handler.first) && handler.first.size() > bestMatch.size()) {
      bestMatch = handler.first;
    }
  }
  if (bestMatch.empty()) {
    handleUnknownCommand(cmd);
  } else {
    cmd.erase(0, bestMatch.size());
    std::vector<std::string> args;
    boost::split(args, cmd, boost::is_any_of(" "), boost::token_compress_on);
    args.insert(args.begin(), bestMatch);
    for (auto& arg : args) {
      arg = Xi::trim(arg);
    }
    args.erase(std::remove_if(args.begin(), args.end(), [](const auto& arg) { return arg.empty(); }), args.end());
    runCommand(args);
  }
}

void ConsoleHandler::handleUnknownCommand(const std::string& cmd) {
  printWarning(std::string{"Unknown command: "} + cmd);
}

void ConsoleHandler::completionCallback(const char* buf, std::vector<std::string>& out) {
  const auto buflen = strlen(buf);
  for (const auto& handler : m_handlers) {
    const auto& cmd = handler.first;
    if (buflen > cmd.size()) {
      continue;
    }
    if (std::memcmp(cmd.c_str(), buf, buflen) == 0) {
      out.push_back(cmd);
    }
  }
}

void ConsoleHandler::quit() {
  try {
    linenoise::AddHistory("quit");
    doQuit();
  } catch (...) {
    /* swallow */
  }
}

void ConsoleHandler::clear() {
  linenoise::linenoiseClearScreen();
}

}  // namespace Common
