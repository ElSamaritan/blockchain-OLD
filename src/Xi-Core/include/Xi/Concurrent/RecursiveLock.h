/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#pragma once

#include <Xi/ExternalIncludePush.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <Xi/ExternalIncludePop.h>

namespace Xi {
namespace Concurrent {
struct RecursiveLock {
  using lock_t = boost::recursive_mutex::scoped_lock;

  RecursiveLock() : mutex{} {
  }
  ~RecursiveLock() = default;

  mutable boost::recursive_mutex mutex;
  operator boost::recursive_mutex&() const {
    return mutex;
  }
};

#define XI_CONCURRENT_RLOCK(X)                               \
  ::Xi::Concurrent::RecursiveLock::lock_t __##X##__RLOCK{X}; \
  (void)__##X##__RLOCK
}  // namespace Concurrent
}  // namespace Xi
