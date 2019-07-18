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

#include <Xi/Global.hh>

#include "Xi/ProofOfWork/IAlgorithm.hpp"

namespace Xi {
namespace ProofOfWork {

struct CNX_v1_Light : IAlgorithm {
  ~CNX_v1_Light() override = default;
  void operator()(Xi::ConstByteSpan blob, ::Crypto::Hash& hash) const override;
};

struct CNX_v1 : IAlgorithm {
  ~CNX_v1() override = default;
  void operator()(Xi::ConstByteSpan blob, ::Crypto::Hash& hash) const override;
};

struct CNX_v1_Heavy : IAlgorithm {
  ~CNX_v1_Heavy() override = default;
  void operator()(Xi::ConstByteSpan blob, ::Crypto::Hash& hash) const override;
};

}  // namespace ProofOfWork
}  // namespace Xi
