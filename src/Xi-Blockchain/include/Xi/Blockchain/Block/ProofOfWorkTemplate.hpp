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
#include <Xi/Span.hpp>
#include <Xi/Byte.hh>
#include <Xi/Blob.hpp>

#include "Xi/Blockchain/Block/Hash.hpp"
#include "Xi/Blockchain/Block/Nonce.hpp"

namespace Xi {
namespace Blockchain {
namespace Block {

struct ProofOfWorkTemplate : Xi::enable_blob_from_this<ProofOfWorkTemplate, Nonce::bytes() + Hash::bytes()> {
  using enable_blob_from_this::enable_blob_from_this;

  const Byte* nonceData() const {
    return this->data();
  }
  Byte* nonceData() {
    return this->data();
  }
  ByteSpan nonceSpan() {
    return Xi::makeSpan(nonceData(), Nonce::bytes());
  }

  const Byte* hashData() const {
    return this->data() + Nonce::bytes();
  }
  Byte* hashData() {
    return this->data() + Nonce::bytes();
  }
};

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi
