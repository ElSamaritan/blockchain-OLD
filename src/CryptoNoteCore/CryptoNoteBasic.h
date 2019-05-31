// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cinttypes>

#include <Xi/Byte.hh>

#include <boost/utility/value_init.hpp>
#include <CryptoNoteCore/CryptoNote.h>

namespace CryptoNote {

/*!
 * \brief generateKeyPair Generates a non-pseudo random key pair
 * \return The generated key pair
 */
KeyPair generateKeyPair();

/*!
 * \brief generateKeyPair Generates a pseudo random determinstic key pair
 * \param seed The seed to use for generation.
 * \return The generated key pair
 *
 * This implementation will always return the same key pair for a fixed seed accross platforms.
 */
KeyPair generateDeterministicKeyPair(Xi::ConstByteSpan seed);

}  // namespace CryptoNote
