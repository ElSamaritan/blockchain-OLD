/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

namespace Xi {
namespace Concurrent {
struct ReadersWriterLock {
  mutable boost::shared_mutex mutex;
  operator boost::shared_mutex&() const { return mutex; }
};

#define XI_CONCURRENT_LOCK_READ(X)                               \
  boost::shared_lock<boost::shared_mutex> __##X##__READ_LOCK{X}; \
  (void)__##X##__READ_LOCK

#define XI_CONCURRENT_LOCK_PREPARE_WRITE(X)                          \
  boost::upgrade_lock<boost::shared_mutex> __##X##__PREPARE_LOCK{X}; \
  (void)__##X##__PREPARE_LOCK

#define XI_CONCURRENT_LOCK_ACQUIRE_WRITE(X)                                                        \
  boost::upgrade_to_unique_lock<boost::shared_mutex> __##X##__ACQUIRE_LOCK{__##X##__PREPARE_LOCK}; \
  (void)__##X##__ACQUIRE_LOCK

#define XI_CONCURRENT_LOCK_WRITE(X)                               \
  boost::unique_lock<boost::shared_mutex> __##X##__WRITE_LOCK{X}; \
  (void)__##X##__WRITE_LOCK

}  // namespace Concurrent
}  // namespace Xi
