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

#include <string>

namespace Xi {
/*!
 * \brief The CrashHandlerConfig struct stores general configuration to handle crashes.
 */
struct CrashHandlerConfig {
  bool IsEnabled;  ///< Enables dump creation, if true a crash dump file will be created once the application crashes.
  bool IsUploadEnabled;  ///< If crash dumps are enabled and this as well the dump will be automatically uploaded to the
                         ///< breakpad server
  std::string OutputPath;   ///< Path to store crash dumps
  std::string Application;  ///< The current application identifier running

  CrashHandlerConfig();
};
}  // namespace Xi
