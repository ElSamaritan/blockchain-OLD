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

#include <CryptoNoteCore/CryptoNoteSerialization.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/DatabaseCacheData.h>
#include <Serialization/SerializationOverloads.h>

namespace CryptoNote {

bool KeyOutputInfo::serialize(ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(publicKey, "public_key"), false);
  XI_RETURN_EC_IF_NOT(s(transactionHash, "transaction_hash"), false);
  XI_RETURN_EC_IF_NOT(s(unlockTime, "unlock_time"), false);
  // TODO consider no varint here
  XI_RETURN_EC_IF_NOT(s(index.packedValue, "index"), false);
  return true;
}

}  // namespace CryptoNote
