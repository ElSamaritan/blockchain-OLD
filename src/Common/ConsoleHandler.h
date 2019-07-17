// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <utility>

#include "ConsoleTools.h"

#include <Serialization/ISerializer.h>

#ifndef _WIN32
#include <sys/select.h>
#endif

namespace Common {

class ConsoleHandler {
 public:
  ConsoleHandler();
  virtual ~ConsoleHandler();

  typedef std::function<bool(const std::vector<std::string>&)> ConsoleCommandHandler;

  std::string getUsage() const;
  void setHandler(const std::string& command, const ConsoleCommandHandler& handler, const std::string& usage = "");
  bool runCommand(const std::vector<std::string>& cmdAndArgs);

  void run(const std::string& prompt, const std::string& datadir);

  virtual std::unique_ptr<CryptoNote::ISerializer> makeOutputSerialiizer() const;

  template <typename _ValueT>
  bool printObject(const _ValueT& object, Common::StringView name = "") {
    auto serializer = this->makeOutputSerialiizer();
    std::cout << "\n";
    XI_RETURN_EC_IF_NOT(serializer->beginObject(""), false);
    XI_RETURN_EC_IF_NOT((*serializer)(const_cast<_ValueT&>(object), name), false);
    XI_RETURN_EC_IF_NOT(serializer->endObject(), false);
    std::cout << std::endl;
    XI_RETURN_SC(true);
  }

 protected:
  virtual void printError(std::string error);
  virtual void printWarning(std::string warn);
  virtual void printMessage(std::string msg);

  virtual void doQuit();

 private:
  typedef std::map<std::string, std::pair<ConsoleCommandHandler, std::string>> CommandHandlersMap;

  void handleCommand(const std::string& cmd);
  void completionCallback(const char*, std::vector<std::string>&);
  void quit();
  void clear();

  std::string m_prompt;
  CommandHandlersMap m_handlers;
};
}  // namespace Common
