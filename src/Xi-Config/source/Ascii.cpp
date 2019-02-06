/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include "Xi/Config/Ascii.h"

#include <Logging/ILogger.h>

namespace {
const std::string ASCIIArt = R"(
                *
                                                                          *
     ________       .__                .__         __________                   __               __
    /  _____/_____  |  | _____  ___  __|__|____    \______   \_______  ____    |__| ____   _____/  |_
   /   \  ___\__  \ |  | \__  \ \  \/  /  \__  \    |     ___/\_  __ \/  _ \   |  |/ __ \_/ ___\   __\
   \    \_\  \/ __ \|  |__/ __ \_>    <|  |/ __ \_  |    |     |  | \(  <_> )  |  \  ___/\  \___|  |
    \______  (____  /____(____  /__/\_ \__(____  /  |____|     |__|   \____/\__|  |\___  >\___  >__|
           \/     \/          \/      \/       \/                          \______|    \/     \/

       *                       ____  ___.___
                               \   \/  /|   |
                        ______  \     / |   |                                           *
                       /_____/  /     \ |   |
                               /___/\  \|___|         *
                *                    \_/                        Welcome to the galaxia project community.
    *

)";

bool replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos) return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

size_t replace_all(std::string& str, const std::string& from, const std::string& to) {
  size_t count = 0;
  while (replace(str, from, to)) ++count;
  return count;
}
}  // namespace

std::string Xi::Config::asciiArt(bool colored) {
  if (!colored) {
    return ASCIIArt;
  } else {
    auto coloredAscii = Logging::BRIGHT_BLUE + ASCIIArt;
    replace_all(coloredAscii, "*", Logging::BRIGHT_YELLOW + "*" + Logging::BRIGHT_BLUE);
    replace(coloredAscii, "Welcome to the galixia project community.",
            Logging::WHITE + "Welcome to the galixia project community." + Logging::BRIGHT_BLUE);
    return coloredAscii;
  }
}
