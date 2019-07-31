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

#include "Xi/App/GeneralOptions.h"

#include <Xi/FileSystem.h>
#include <Xi/Version/Version.h>
#include <Xi/Version/BuildInfo.h>
#include <Xi/Version/ProjectInfo.h>
#include <Xi/Version/License.h>

#include <Serialization/ConsoleOutputSerializer.hpp>
#include <Common/Util.h>
#include <CommonCLI/CommonCLI.h>

#include <iostream>

namespace Xi {
namespace App {

void GeneralOptions::loadEnvironment(Environment &env) {
  env(devMode(), "DEV_MODE");
}

void GeneralOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("general")
      ("help", "Display this help message")
      ("version", "Output software version information")
      ("vversion", "Output verbose software version information")
      ("osversion", "Output Operating System version information")
  ;
  // clang-format on

  if (CommonCLI::isDevVersion()) {
    // clang-format off
    options.add_options("development")
      ("dev-mode", "Indicates you are aware of running a development version. "
                   "You must provide this flag in order to run the application.",
        cxxopts::value<bool>(devMode())->default_value(devMode() ? "true" : "false")->implicit_value("true"))
    ;
    // clang-format on
  }
}

bool GeneralOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options);
  if (result.count("help") > 0) {
    std::cout << options.help({}) << std::endl;
    XI_RETURN_SC(true);
  } else if (result.count("version") > 0) {
    std::cout << "v" << APP_VERSION << " (" << BUILD_COMMIT_ID << ")" << std::endl;
    XI_RETURN_SC(true);
  } else if (result.count("vversion") > 0) {
    CryptoNote::ConsoleOutputSerializer ser{std::cout};
    auto ver = CommonCLI::versionInformation();
    std::cout << std::endl;
    (void)ser(ver, "version");
    std::cout << std::endl;
    XI_RETURN_SC(true);
  } else if (result.count("osversion") > 0) {
    std::cout << Tools::get_os_version_string() << std::endl;
    XI_RETURN_SC(true);
  } else {
    if (CommonCLI::isDevVersion()) {
      if (!devMode()) {
        std::cerr << "--dev-mode flag missing" << std::endl;
        XI_RETURN_EC(true);
      }
    }
    XI_RETURN_SC(false);
  }
}

}  // namespace App
}  // namespace Xi
