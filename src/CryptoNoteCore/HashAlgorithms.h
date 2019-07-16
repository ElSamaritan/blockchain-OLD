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

#pragma once

#include <memory>
#include <string_view>
#include <string>
#include <vector>

#include <Xi/Byte.hh>

#include "CryptoNoteCore/CachedBlock.h"

namespace CryptoNote {
namespace Hashes {

struct IProofOfWorkAlgorithm {
  virtual ~IProofOfWorkAlgorithm() = default;

  virtual void operator()(Xi::ConstByteSpan blob, Crypto::Hash& hash) const = 0;
};

struct CNX_v1 : IProofOfWorkAlgorithm {
  ~CNX_v1() override = default;
  void operator()(Xi::ConstByteSpan blob, Crypto::Hash& hash) const override;
};

struct Sha2_256 : IProofOfWorkAlgorithm {
  ~Sha2_256() override = default;
  void operator()(Xi::ConstByteSpan blob, Crypto::Hash& hash) const override;
};

struct Keccak : IProofOfWorkAlgorithm {
  ~Keccak() override = default;
  void operator()(Xi::ConstByteSpan blob, Crypto::Hash& hash) const override;
};

std::shared_ptr<IProofOfWorkAlgorithm> makeProofOfWorkAlgorithm(std::string_view id);
std::vector<std::string> supportedProofOfWorkAlgorithms();

}  // namespace Hashes
}  // namespace CryptoNote
