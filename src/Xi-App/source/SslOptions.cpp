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

#include "Xi/App/SslOptions.h"

#include <Xi/FileSystem.h>

namespace Xi {
namespace App {

void SslOptions::loadEnvironment(Environment &env) {
  // clang-format off
  env
    (enabled(), "SSL")
    (rootDir(), "SSL_DIR")
  ;
  // clang-format on
}

void SslOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("ssl")
        ("ssl", "Enables and enforces SSL usage to secure your privacy.",
            cxxopts::value<bool>(enabled())->implicit_value("true"))

        ("ssl-dir", "default directory to lookup all ssl related files.",
            cxxopts::value<std::string>(rootDir())->default_value(rootDir()));
  // clang-format on
}

bool SslOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  XI_RETURN_SC_IF_NOT(enabled(), false);
  XI_RETURN_SC(false);
}

void SslOptions::configure(Http::SSLConfiguration &config) const {
  config.setEnabled(enabled());
  config.setRootPath(rootDir());
}

}  // namespace App
}  // namespace Xi
