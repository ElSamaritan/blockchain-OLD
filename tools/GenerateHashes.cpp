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

#include <iostream>
#include <string>

#include <crypto/cnx/cnx.h>
#include <Common/StringTools.h>

std::string makeHash(std::string data, bool forceSoftwareAES) {
  Crypto::CNX::Hash_v0 hashFn;
  Crypto::Hash hash;
  hashFn(data.c_str(), data.length(), hash);
  std::string hashString;
  hashString.resize(sizeof(hash));
  for (std::size_t i = 0; i < sizeof(hash); ++i) hashString[i] = reinterpret_cast<char*>(&hash)[i];
  return Common::base64Encode(hashString);
}

int main(int, char**) {
  std::string currentLine;
  while (!std::cin.eof()) {
    std::getline(std::cin, currentLine, '\n');
    if (currentLine.empty()) continue;
    std::string data = Common::base64Decode(currentLine);
    std::cout << makeHash(data, true) << std::endl;
    std::cout << makeHash(data, false) << std::endl;
  }
}
