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

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

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
 * \brief emplaceCLIOptions will add common options for CLI applications to the option parser interface
 * \param options The parser that will handle the options
 */
void emplaceCLIOptions(cxxopts::Options& options);

/*!
 * \brief handleCLIOptions handles common options given by the CLI
 * \param option The options that have been parsed
 * \param result The parsed options result
 * \return true if the application should exit, otherwise false
 */
bool handleCLIOptions(cxxopts::Options& options, const cxxopts::ParseResult& result);
}  // namespace CommonCLI
