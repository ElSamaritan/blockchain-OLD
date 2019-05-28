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

#include "CryptoNoteBasic.h"
#include "crypto/chacha8.h"
#include "Serialization/ISerializer.h"
#include "crypto/crypto.h"

namespace Crypto {

bool serialize(chacha8_iv& chacha, Common::StringView name, CryptoNote::ISerializer& serializer);
bool serialize(EllipticCurveScalar& ecScalar, Common::StringView name, CryptoNote::ISerializer& serializer);
bool serialize(EllipticCurvePoint& ecPoint, Common::StringView name, CryptoNote::ISerializer& serializer);

}  // namespace Crypto

namespace CryptoNote {

struct AccountKeys;

enum class SerializationTag : uint8_t { Base = 0xff, Key = 0x2, Transaction = 0xcc, Block = 0xbb };

bool serialize(TransactionPrefix& txP, ISerializer& serializer);
bool serialize(Transaction& tx, ISerializer& serializer);
bool serialize(BaseTransaction& tx, ISerializer& serializer);
bool serialize(TransactionInput& in, ISerializer& serializer);
bool serialize(TransactionOutput& in, ISerializer& serializer);

bool serialize(BaseInput& gen, ISerializer& serializer);
bool serialize(KeyInput& key, ISerializer& serializer);

bool serialize(TransactionOutput& output, ISerializer& serializer);
bool serialize(TransactionOutputTarget& output, ISerializer& serializer);
bool serialize(KeyOutput& key, ISerializer& serializer);

bool serialize(BlockHeader& header, ISerializer& serializer);
bool serialize(BlockTemplate& block, ISerializer& serializer);

bool serialize(AccountPublicAddress& address, ISerializer& serializer);
bool serialize(FeeAddress& addr, ISerializer& serializer);
bool serialize(AccountKeys& keys, ISerializer& s);

bool serialize(KeyPair& keyPair, ISerializer& serializer);
bool serialize(RawBlock& rawBlock, ISerializer& serializer);
bool serialize(FullBlock& block, ISerializer& serializer);
bool serialize(LiteBlock& block, ISerializer& serializer);

}  // namespace CryptoNote
