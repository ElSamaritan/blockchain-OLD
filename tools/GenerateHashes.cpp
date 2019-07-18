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

#include <iostream>
#include <string>

#include <Xi/ProofOfWork/Cnx.hpp>
#include <Common/StringTools.h>

std::string makeHash(std::string data) {
  Xi::ProofOfWork::CNX_v1_Light hashFn;
  Crypto::Hash hash;
  hashFn(Xi::asByteSpan(data), hash);
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
    std::cout << makeHash(data) << std::endl;
  }
}
