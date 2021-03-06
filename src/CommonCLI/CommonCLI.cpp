﻿/* ============================================================================================== *
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

#include "CommonCLI.h"

#include <sstream>
#include <iostream>

#include <Xi/Version/Version.h>
#include <Xi/Version/BuildInfo.h>
#include <Xi/Version/ProjectInfo.h>
#include <Xi/Version/License.h>
#include <Xi/Config/Ascii.h>
#include <Xi/Config.h>

#include <Common/Util.h>

namespace {
// clang-format off
const std::string DevelpmentVersionHeader = R"(
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   ATTENTION   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !                                                                                       !
   ! You are running a development version! The program may contain bugs or is not         !
   ! compatible with the main network. In case you accidentally ran into this version and  !
   ! do not want to use it to for testing purposes you should visit our GitLab page        !
   !     https://gitlab.com/galaxia-project/blockchain                                     !
   ! . Or if you want to build xi yourself make sure you are building from the master      !
   ! branch.                                                                               !
   !                                                                                       !
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   ATTENTION   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)";
// clang-format on
}  // namespace

#if defined(XI_USE_BREAKPAD)
#include <Xi/CrashHandler.h>

namespace {
Xi::CrashHandlerConfig BreakpadConfig;
}
#endif  // XI_USE_BREAKPAD

std::string CommonCLI::header(const CryptoNote::Currency& currency, bool colored) {
  std::stringstream programHeader;
  programHeader << std::endl
                << Xi::Config::asciiArt(colored) << std::endl
                << " " << currency.coin().name() << " v" << APP_VERSION << " (" << BUILD_COMMIT_ID << ")" << std::endl
                << " This software is distributed under the General Public License v3.0" << std::endl
                << std::endl
                << " " << PROJECT_COPYRIGHT << std::endl
                << std::endl
                << " Additional Copyright(s) may apply, please see the included LICENSE file for more information."
                << std::endl;
  if (!currency.general().licenseUrl().empty()) {
    programHeader << " If you did not receive a copy of the LICENSE, please visit:" << std::endl
                  << " " << currency.general().licenseUrl() << std::endl
                  << std::endl;
  }

  if (isDevVersion()) {
    programHeader << DevelpmentVersionHeader << std::endl;
  }

  return programHeader.str();
}

bool CommonCLI::isDevVersion() {
  return
#if defined(BUILD_CHANNEL_STABLE) || defined(BUILD_CHANNEL_BETA)
      false
#else
      true
#endif
      ;
}

std::string CommonCLI::devWarning() {
  return R"(
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   ATTENTION   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !                                                                                       !
   ! You are running a development version! The program may contain bugs or is not         !
   ! compatible with the main network. In case you accidentally ran into this version and  !
   ! do not want to use it to for testing purposes you should visit our GitLab page        !
   !     https://gitlab.com/galaxia-project/blockchain                                     !
   ! . Or if you want to build xi yourself make sure you are building from the master      !
   ! branch.                                                                               !
   !                                                                                       !
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   ATTENTION   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)";
}

std::string CommonCLI::insecureClientWarning() {
  return R"(
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   WARNING   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !                                                                                       !
   ! Your client is not set up to be the most secure. To increase your security please     !
   ! read our tutorial on setting up SSL correctly.                                        !
   !     https://doc.galaxia-project.com/ssl/                                              !
   !                                                                                       !
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   WARNING   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)";
}

std::string CommonCLI::insecureServerWarning() {
  return R"(
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   WARNING   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !                                                                                       !
   ! Your server is not set up to be the most secure. To increase your security please     !
   ! read our tutorial on setting up SSL correctly.                                        !
   !     https://doc.galaxia-project.com/ssl/                                              !
   !                                                                                       !
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   WARNING   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)";
}

void CommonCLI::emplaceCLIOptions(cxxopts::Options& options) {
  // clang-format off
  options.add_options("general")
      ("help", "Display this help message")
      ("version", "Output software version information")
      ("vversion", "Output verbose software version information")
      ("os-version", "Output Operating System version information");

#if defined(XI_USE_BREAKPAD)
  options.add_options("breakpad")
      ("breakpad-enable", "Enables creation of crash dumps to help developers reconstruct bugs occuring in release builds.",
       cxxopts::value<bool>(BreakpadConfig.IsEnabled)->implicit_value("true")->default_value("false"))

      ("breakpad-out", "Output directory for storing crash dumps",
       cxxopts::value<std::string>(BreakpadConfig.OutputPath)->default_value(BreakpadConfig.OutputPath));

  if(isDevVersion())
  {
    options.add_options("breakpad")
      ("breakpad-upload", "Enables auto upload of crash dumps to the galaxia project breakpad server.",
       cxxopts::value<bool>(BreakpadConfig.IsUploadEnabled)->implicit_value("true")->default_value("false"));
  }

#endif  // XI_USE_BREAKPAD

  options.add_options("license")
      ("license", "Print the project license and exits.")
      ("third-party","Prints a summary of all third party libraries used by this project.")
      ("third-party-licenses", "Prints every license included by third party libraries used by this project.");

  if(isDevVersion()) {
    options.add_options("development")("dev-mode", "Indicates you are aware of running a development version. "
                                                   "You must provide this flag in order to run the application.");
  }
  // clang-format on
}

bool CommonCLI::handleCLIOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) {
  if (result.count("help")) {
    std::cout << options.help({}) << std::endl;
    return true;
  } else if (result.count("version")) {
    std::cout << "v" << APP_VERSION << " (" << BUILD_COMMIT_ID << ")" << std::endl;
    return true;
  } else if (result.count("vversion")) {
    std::cout << verboseVersionInformation() << std::endl;
    return true;
  } else if (result.count("license")) {
    std::cout << Xi::Version::license() << std::endl;
    return true;
  } else if (result.count("third-party")) {
    std::cout << Xi::Version::thirdParty() << std::endl;
    return true;
  } else if (result.count("third-party-licenses")) {
    std::cout << Xi::Version::thirdPartyLicense() << std::endl;
    return true;
  } else if (result.count("os-version")) {
    std::cout << "OS: " << Tools::get_os_version_string() << std::endl;
    return true;
  } else if (isDevVersion() && (result.count("dev-mode") == 0)) {
    std::cout << "\n You are using a development version and did not provide the --dev-mode flag. Exiting..."
              << std::endl;
    return true;
  } else {
    return false;
  }
}

void* CommonCLI::make_crash_dumper(const std::string& applicationId) {
#if defined(XI_USE_BREAKPAD)
  if (BreakpadConfig.IsEnabled) {
    BreakpadConfig.Application = applicationId;
    if (!isDevVersion())
      BreakpadConfig.IsUploadEnabled = false;
    return new Xi::CrashHandler(BreakpadConfig);
  }
#else
  (void)applicationId;
#endif  // XI_USE_BREAKPAD
  return nullptr;
}

std::string CommonCLI::verboseVersionInformation() {
  std::stringstream str{};
  // clang-format off
  str
      << "Version\t\t: v" << APP_VERSION
      << "\nChannel\t\t: " << BUILD_CHANNEL
      << "\nBuild Type\t: " << BUILD_TYPE
      << "\nBuild Timestamp\t: " << BUILD_TIMESTAMP
      << "\nGit Commit\t: " << BUILD_COMMIT_ID
      << "\nGit Branch\t: " << BUILD_BRANCH
      << "\nC Compiler\t: " << BUILD_C_COMPILER_INFO
      << "\nCXX Compiler\t: " << BUILD_CXX_COMPILER_INFO
      << "\nBoost Version\t: " << BUILD_BOOST_VERSION
      << "\nOpenSSL Version\t: " << BUILD_OPENSSL_VERSION
      << "\n\n"
      << PROJECT_COPYRIGHT;
  // clang-format on
  return str.str();
}

CommonCLI::VersionInformation CommonCLI::versionInformation() {
  VersionInformation reval{};
  reval.version = APP_VERSION;
  reval.copyright = PROJECT_COPYRIGHT;
  reval.build.channel = BUILD_CHANNEL;
  reval.build.type = BUILD_TYPE;
  reval.build.dev = isDevVersion();
  reval.build.compiler.cc = BUILD_C_COMPILER_INFO;
  reval.build.compiler.cxx = BUILD_CXX_COMPILER_INFO;
  reval.git.commit = BUILD_COMMIT_ID;
  reval.git.branch = BUILD_BRANCH;
  reval.external.boost = BUILD_BOOST_VERSION;
  reval.external.openssl = BUILD_OPENSSL_VERSION;
  return reval;
}
