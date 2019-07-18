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

#include "Xi/ProofOfWork/ProofOfWork.hpp"

#include "Xi/ProofOfWork/Cnx.hpp"
#include "Xi/ProofOfWork/Sha2-256.hpp"
#include "Xi/ProofOfWork/Keccak.hpp"

namespace Xi {
namespace ProofOfWork {

std::shared_ptr<IAlgorithm> makeAlgorithm(std::string_view id) {
  if (id == "CNX-v1-Light") {
    return std::make_shared<CNX_v1_Light>();
  } else if (id == "CNX-v1") {
    return std::make_shared<CNX_v1>();
  } else if (id == "CNX-v1-Heavy") {
    return std::make_shared<CNX_v1_Heavy>();
  } else if (id == "SHA2-256") {
    return std::make_shared<SHA2_256>();
  } else if (id == "Keccak") {
    return std::make_shared<Keccak>();
  } else {
    return nullptr;
  }
}

std::vector<std::string> supportedAlgorithms() {
  return {
      "CNX-v1-Light", "CNX-v1", "CNX-v1-Heavy", "SHA2-256", "Keccak",
  };
}

}  // namespace ProofOfWork
}  // namespace Xi
