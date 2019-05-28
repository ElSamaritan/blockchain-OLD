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

#include "Serialization/SerializationOverloads.h"

#include <limits>

namespace CryptoNote {

bool serializeBlockHeight(ISerializer& s, uint32_t& blockHeight, Common::StringView name) {
  return s(blockHeight, name);
}

bool serializeGlobalOutputIndex(ISerializer& s, uint32_t& globalOutputIndex, Common::StringView name) {
  return serializeBlockHeight(s, globalOutputIndex, name);
}

}  // namespace CryptoNote
