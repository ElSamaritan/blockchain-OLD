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

#include "Xi/App/BreakpadOptions.h"

#include <Xi/FileSystem.h>

namespace Xi {
namespace App {

void BreakpadOptions::loadEnvironment(Environment &env) {
  // clang-format off
  env
    (enabled(), "BREAKPAD")
    (outputPath(), "BREAKPAD_OUT")
  ;
  // clang-format on
}

void BreakpadOptions::emplaceOptions(cxxopts::Options &options) {
#if defined(XI_USE_BREAKPAD)
  // clang-format off
  options.add_options("breakpad")
    ("breakpad-enable", "Enables creation of crash dumps to help developers reconstruct bugs occuring in release builds.",
       cxxopts::value<bool>(enabled())->implicit_value("true")->default_value(enabled() ? "true" : "false"))

    ("breakpad-out", "Output directory for storing crash dumps",
       cxxopts::value<std::string>(outputPath())->default_value(outputPath()));
  // clang-format on
#else
  XI_UNUSED(options);
#endif
}

bool BreakpadOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
#if defined(XI_USE_BREAKPAD)
  XI_RETURN_SC_IF_NOT(enabled(), false);
  FileSystem::ensureDirectoryExists(outputPath()).throwOnError();
#else
  enabled(false);
#endif
  XI_RETURN_SC(false);
}

}  // namespace App
}  // namespace Xi
