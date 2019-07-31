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

#include "Xi/App/LicenseOptions.h"

#include <iostream>

#include <Xi/Version/License.h>

namespace Xi {
namespace App {

void LicenseOptions::loadEnvironment(Environment &env) {
  XI_UNUSED(env);
}

void LicenseOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("license")
    ("license", "Print the project license and exits.")
    ("license-third-party","Prints a summary of all third party libraries used by this project.")
    ("license-third-party-verbose", "Prints every license included by third party libraries used by this project.")
  ;
  // clang-format on
}

bool LicenseOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options);
  if (result.count("license") > 0) {
    std::cout << Version::license() << std::endl;
    XI_RETURN_SC(true);
  } else if (result.count("license-third-party") > 0) {
    std::cout << Version::thirdParty() << std::endl;
    XI_RETURN_SC(true);
  } else if (result.count("license-third-party-verbose") > 0) {
    std::cout << Version::thirdPartyLicense() << std::endl;
    XI_RETURN_SC(true);
  } else {
    XI_RETURN_SC(false);
  }
}

}  // namespace App
}  // namespace Xi
