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

namespace CommonCLI {
/*!
 * \brief header returns an appropiate header to display including a message telling you you are
 * on a testing version, if so
 * \return a header to display at startup
 */
std::string header();

/*!
 * \brief checks wheter this version was built from non master branch source code
 * \return true if this version is not built from master branch, otherwise false
 */
bool isDevVersion();

/*!
 * \brief insecureClientWarning returns a message to be printed for insecure client setups
 */
std::string insecureClientWarning();

/*!
 * \brief insecureClientWarning returns a message to be printed for insecure server setups
 */
std::string insecureServerWarning();

/*!
 * \brief verifyDevelopmentExecution checks whether the current version is build from master or if
 * not whether the user provided the --dev-mode flag.
 *
 * If this is a development version and the --dev-mode flag is not provided it will print an
 * error message and exit the program.
 */
void verifyDevExecution(int& argc, char** argv);
}  // namespace CommonCLI
