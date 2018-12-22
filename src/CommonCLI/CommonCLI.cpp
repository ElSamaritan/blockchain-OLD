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

#include "CommonCLI.h"

#include <sstream>
#include <iostream>

#include "version.h"
#include "config/Ascii.h"
#include "config/CryptoNoteConfig.h"

// clang-format off
const std::string DevelpmentVersionHeader = R"(
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   ATTENTION   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!                                                                                       !
! You are running a development version! The program may contain bugs or is not         !
! compatible with the mainnet. In case you accidentally ran into this version and do    !
! not want to use it to for testing purposes you should visit our github page           !
!     https://gitlab.com/galaxia-project/blockchain/xi                                  !
! . Or if you want to build xi yourself make sure you are building from the master      !
! branch.                                                                               !
!                                                                                       !
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   ATTENTION   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)";
// clang-format on

std::string CommonCLI::header() {
  std::stringstream programHeader;
  programHeader << std::endl
                << asciiArt << std::endl
                << " " << CryptoNote::Config::Coin::name() << " v" << PROJECT_VERSION_LONG << std::endl
                << " This software is distributed under the General Public License v3.0" << std::endl
                << std::endl
                << " " << PROJECT_COPYRIGHT << std::endl
                << std::endl
                << " Additional Copyright(s) may apply, please see the included LICENSE file for more information."
                << std::endl;
  if (!CryptoNote::Config::Coin::licenseUrl().empty()) {
    programHeader << " If you did not receive a copy of the LICENSE, please visit:" << std::endl
                  << " " << CryptoNote::Config::Coin::licenseUrl() << std::endl
                  << std::endl;
  }

  if (isDevVersion()) {
    programHeader << DevelpmentVersionHeader << std::endl;
  }

  return programHeader.str();
}

bool CommonCLI::isDevVersion() {
  return
#ifdef XI_DEV_VERSION
      true
#else
      false
#endif
      ;
}

void CommonCLI::verifyDevExecution(int& argc, char** argv) {
  const std::string devModeFlag{"--dev-mode"};
  for (int i = 1; i < argc; ++i) {
    if (std::string{argv[i]} == devModeFlag) {
      for (int k = 0; i + k < argc - 1; ++k) {
        argv[i + k] = argv[i + k + 1];
      }
      argc -= 1;
      return;
    }
  }

  if (isDevVersion()) {
    std::cout << header()
              << "\n You are using a development version and did not provide the --dev-mode flag. Exiting..."
              << std::endl;
    exit(-1);
  }
}

std::string CommonCLI::insecureClientWarning() {
  return R"(
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   WARNING   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !                                                                                       !
   ! Your client is not setup to be most secure. To increase you secuirity please read our !
   ! tutorial on setting up ssl correctly.                                                 !
   !     https://galaxia-project.gitlab.io/end-user-documentation/ssl                      !
   !                                                                                       !
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   WARNING   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)";
}

std::string CommonCLI::insecureServerWarning() {
  return R"(
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   WARNING   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !                                                                                       !
   ! Your sever is not setup to be most secure. To increase you secuirity please read our  !
   ! tutorial on setting up ssl correctly.                                                 !
   !     https://galaxia-project.gitlab.io/end-user-documentation/ssl                      !
   !                                                                                       !
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   WARNING   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
)";
}
