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

#pragma once

#include <memory>
#include <string>

#include "Xi/CrashHandlerConfig.h"

namespace Xi {
struct _CrashHandler_Impl;

/*!
 * \brief The CrashHandler class handles crashes of the application by creating crash dumps and uploading them
 *
 * In order to create crash dumps you need to create an instance of this class and store it as long as possible. This
 * class itself will register for termination calls and try to create/upload crash dumps just in time before the
 * application exits.
 */
class CrashHandler {
 public:
  /*!
   * \brief CrashHandler constructs a new crash handler
   * \param config The configuration specifying where to upload crash dumps and wheter to upload them
   */
  CrashHandler(const CrashHandlerConfig& config);
  CrashHandler(const CrashHandler&) = delete;
  CrashHandler& operator=(const CrashHandler&) = delete;
  ~CrashHandler() = default;

 private:
  std::shared_ptr<_CrashHandler_Impl> m_impl;
};

}  // namespace Xi
