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

#include <string>

#include "Logging/ILogger.h"
#include "Logging/Level.h"
#include "Logging/LoggerMessage.h"

namespace Logging {

class LoggerRef {
 public:
  LoggerRef(ILogger& logger, const std::string& category);
  LoggerMessage operator()(Level level = Level::Info) const;
  LoggerMessage operator()(Level level, const std::string& color) const;
  ILogger& getLogger() const;

 private:
  ILogger* logger;
  std::string category;
};

}  // namespace Logging
