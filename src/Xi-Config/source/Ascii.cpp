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

       *                       ____  ___.___                   *
                               \   \/  /|   |
                                \     / |   |                                           *
                                /     \ |   |
                               /___/\  \|___|         *
                *                    \_/                        Welcome to the galaxia project community.
    *

)";

size_t replace_all(std::string& str, const std::string& from, const std::string& to) {
  size_t count = 0;
  for (size_t start_pos = str.find(from); start_pos != std::string::npos; start_pos = str.find(from, start_pos)) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
    count += 1;
  }
  return count;
}

void colorize(std::string& str, const std::string& target, const std::string& col) {
  replace_all(str, target, col + target + col);
}
}  // namespace

std::string Xi::Config::asciiArt(bool colored) {
  if (!colored) {
    return ASCIIArt;
  } else {
    static const std::string _GalaxiaProjectColor = Logging::CYAN;
    static const std::string _XiColor = Logging::GREEN;
    static const std::string _StarsColor = Logging::YELLOW;
    static const std::string _WelcomeColor = Logging::DEFAULT;

    auto coloredAscii = _GalaxiaProjectColor + ASCIIArt;
    colorize(coloredAscii, "*", _StarsColor);
    colorize(coloredAscii,
             "________       .__                .__         __________                   __               __",
             _GalaxiaProjectColor);
    colorize(coloredAscii,
             "/  _____/_____  |  | _____  ___  __|__|____    \\______   \\_______  ____    |__| ____   _____/  |_",
             _GalaxiaProjectColor);
    colorize(coloredAscii,
             "/   \\  ___\\__  \\ |  | \\__  \\ \\  \\/  /  \\__  \\    |     ___/\\_  __ \\/  _ \\   |  |/ __ \\_/ "
             "___\\   __\\",
             _GalaxiaProjectColor);
    colorize(
        coloredAscii,
        "\\    \\_\\  \\/ __ \\|  |__/ __ \\_>    <|  |/ __ \\_  |    |     |  | \\(  <_> )  |  \\  ___/\\  \\___|  |",
        _GalaxiaProjectColor);
    colorize(coloredAscii,
             "\\______  (____  /____(____  /__/\\_ \\__(____  /  |____|     |__|   \\____/\\__|  |\\___  >\\___  >__|",
             _GalaxiaProjectColor);
    colorize(coloredAscii,
             "\\/     \\/          \\/      \\/       \\/                          \\______|    \\/     \\/",
             _GalaxiaProjectColor);

    colorize(coloredAscii, "____  ___.___ ", _XiColor);
    colorize(coloredAscii, "\\   \\/  /|   |", _XiColor);
    colorize(coloredAscii, " \\     / |   |", _XiColor);
    colorize(coloredAscii, " /     \\ |   |", _XiColor);
    colorize(coloredAscii, "/___/\\  \\|___|", _XiColor);
    colorize(coloredAscii, "      \\_/", _XiColor);
    colorize(coloredAscii, "Welcome to the galaxia project community.", _WelcomeColor);
    return coloredAscii;
  }
}
