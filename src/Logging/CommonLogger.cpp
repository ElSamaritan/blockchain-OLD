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

#include "CommonLogger.h"

#include <Xi/Global.hh>

#include <chrono>
#include <utility>

#if !defined(NDEBUG)
#include <iostream>
#endif

namespace Logging {

namespace {

std::string formatPattern(const std::string& pattern, const std::string& category, Level level,
                          boost::posix_time::ptime time) {
  std::stringstream s;

  for (const char* p = pattern.c_str(); p && *p != 0; ++p) {
    if (*p == '%') {
      ++p;
      switch (*p) {
        case 0:
          break;
        case 'C':
          s << category;
          break;
        case 'D':
          s << time.date();
          break;
        case 'T':
          s << time.time_of_day();
          break;
        case 'L':
          s << std::setw(7) << std::left << ILogger::LEVEL_NAMES[level];
          break;
        default:
          s << *p;
      }
    } else {
      s << *p;
    }
  }

  return s.str();
}

}  // namespace

void CommonLogger::operator()(const std::string& category, Level level, boost::posix_time::ptime time,
                              const std::string& body) {
  if (level == Logging::None || logLevel.load(std::memory_order_consume) == Logging::None) {
    return;
  }

  if (level > logLevel.load(std::memory_order_consume)) {
    return;
  }

  if (m_shutdown.load(std::memory_order_consume)) {
    return;
  }

  LogContext ctx{};
  ctx.category = category;
  ctx.level = level;
  ctx.time = time;
  ctx.thread = std::this_thread::get_id();
  ctx.body = body;

  {
    std::lock_guard<std::mutex> _{m_queueGuard};
    XI_UNUSED(_);
    m_queue.emplace(std::move(ctx));
  }
}

void CommonLogger::setPattern(const std::string& _pattern) {
  XI_CONCURRENT_LOCK_WRITE(m_configGuard);
  this->pattern = _pattern;
}

void CommonLogger::enableCategory(const std::string& category) {
  XI_CONCURRENT_LOCK_WRITE(m_configGuard);
  disabledCategories.erase(category);
}

void CommonLogger::disableCategory(const std::string& category) {
  XI_CONCURRENT_LOCK_WRITE(m_configGuard);
  disabledCategories.insert(category);
}

void CommonLogger::setMaxLevel(Level level) {
  logLevel = level;
}

CommonLogger::CommonLogger(Level level) : pattern("%D %T [%C] "), logLevel(level) {
  m_detached = std::thread{[this]() { loopQueue(); }};
}

void CommonLogger::doLogString(const std::string& message) {
  XI_UNUSED(message);
}

void CommonLogger::doLogContext(CommonLogger::LogContext context) {
  if (disabledCategories.count(context.category) > 0) {
    return;
  }

  if (!pattern.empty()) {
    size_t insertPos = 0;
    if (!context.body.empty() && context.body[0] == ILogger::COLOR_DELIMETER) {
      size_t delimPos = context.body.find(ILogger::COLOR_DELIMETER, 1);
      if (delimPos != std::string::npos) {
        insertPos = delimPos + 1;
      }
    }

    context.body.insert(insertPos, formatPattern(pattern, context.category, context.level, context.time));
  }

  doLogString(context.body);
}

CommonLogger::~CommonLogger() {
  try {
    m_shutdown.store(true, std::memory_order_release);
    m_detached.join();
  } catch (...) {
    /* */
  }
}

void CommonLogger::loopQueue() {
  while (!m_shutdown.load(std::memory_order_consume)) {
    std::queue<LogContext> toLog;
    {
      std::lock_guard<std::mutex> _{m_queueGuard};
      XI_UNUSED(_);
      toLog = std::move(m_queue);
      m_queue = std::queue<LogContext>{};
    }
    if (toLog.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds{10});
    } else {
      while (!toLog.empty()) {
        logContext(std::move(toLog.front()));
        toLog.pop();
      }
    }
  }
}

void CommonLogger::logContext(CommonLogger::LogContext context) {
  try {
    XI_CONCURRENT_LOCK_READ(m_configGuard);
    doLogContext(context);
  }
#if !defined(NDEBUG)
  catch (std::exception& e) {
    std::cout << "log threw: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "log threw: UNKNOWN" << std::endl;
  }
}
#else
  catch (...) {
    /* */
  }
#endif
}  // namespace Logging
