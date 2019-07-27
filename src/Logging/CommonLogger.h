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

#include <set>
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <variant>

#include <Xi/Concurrent/ReadersWriterLock.h>

#include "ILogger.h"

namespace Logging {

class CommonLogger : public ILogger {
 protected:
  struct LogContext {
    std::string category;
    Level level;
    boost::posix_time::ptime time;
    std::thread::id thread;
    std::variant<std::string, std::shared_ptr<ILogObject>> body;

    LogContext() = default;
    ~LogContext() = default;
  };

 public:
  ~CommonLogger() override;

  virtual void operator()(const std::string& category, Level level, boost::posix_time::ptime time,
                          const std::string& body) override;
  virtual void operator()(const std::string& category, Level level, boost::posix_time::ptime time,
                          std::shared_ptr<ILogObject> obj) override;

  virtual void enableCategory(const std::string& category);
  virtual void disableCategory(const std::string& category);
  virtual void setMaxLevel(Level level);

  void setPattern(const std::string& pattern);

 protected:
  std::set<std::string> disabledCategories;
  std::string pattern;
  Xi::Concurrent::ReadersWriterLock m_configGuard;

  std::atomic<Level> logLevel;

  std::thread m_detached{};
  std::queue<LogContext> m_queue{};
  std::atomic_bool m_shutdown{false};
  std::mutex m_queueGuard;

  CommonLogger(Level level);

  std::string makeContextPrefix(const LogContext& context) const;

  virtual void doLogString(const std::string& message);
  virtual void doLogString(LogContext context, const std::string& message);
  virtual void doLogObject(LogContext context, ILogObject& object);
  virtual void doLogObject(ILogObject& object);
  virtual void doLogContext(LogContext context);

 private:
  bool isFiltered(Level level) const;
  void loopQueue();
  void logContext(LogContext context);
  void logString(const std::string& str);
  void logString(LogContext context, const std::string& str);
  void logObject(LogContext context, ILogObject& object);
  void logObject(ILogObject& object);
};

}  // namespace Logging
