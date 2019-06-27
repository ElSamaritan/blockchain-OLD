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

#include <System/Dispatcher.h>

#include "Xi/Concurrent/IDispatcher.h"

namespace Xi {
namespace Concurrent {
/*!
 * \brief The SystemDispatcher class implements a thread working queue based on the system library dispatcher
 * implementation
 */
class SystemDispatcher : public IDispatcher {
 public:
  /*!
   * \brief SystemDispatcher wraps a system library dispatcher
   * \param dispatcher the system library dispatcher to wrap
   */
  SystemDispatcher(System::Dispatcher& dispatcher);
  ~SystemDispatcher() override = default;

  /*!
   * \brief post enqueues the task to the system library dispatcher
   * \param cn the task to execute
   */
  void post(std::function<void()> cn) override;

 private:
  System::Dispatcher& m_dispatcher;
};
}  // namespace Concurrent
}  // namespace Xi
