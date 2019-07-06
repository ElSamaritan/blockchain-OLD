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
#include "CryptoNoteCore/Blockchain/RawBlock.h"
#include "crypto/chacha8.h"
#include "Serialization/ISerializer.h"
#include "crypto/crypto.h"

namespace Crypto {

[[nodiscard]] bool serialize(chacha8_iv& chacha, Common::StringView name, CryptoNote::ISerializer& serializer);
[[nodiscard]] bool serialize(EllipticCurveScalar& ecScalar, Common::StringView name,
                             CryptoNote::ISerializer& serializer);
[[nodiscard]] bool serialize(EllipticCurvePoint& ecPoint, Common::StringView name, CryptoNote::ISerializer& serializer);

}  // namespace Crypto

namespace CryptoNote {

struct AccountKeys;

[[nodiscard]] bool serialize(TransactionPrefix& txP, ISerializer& serializer);
[[nodiscard]] bool serialize(Transaction& tx, ISerializer& serializer);
[[nodiscard]] bool serialize(TransactionOutput& in, ISerializer& serializer);

[[nodiscard]] bool serialize(BaseInput& gen, ISerializer& serializer);
[[nodiscard]] bool serialize(KeyInput& key, ISerializer& serializer);

[[nodiscard]] bool serialize(TransactionOutput& output, ISerializer& serializer);
[[nodiscard]] bool serialize(KeyOutput& key, ISerializer& serializer);

[[nodiscard]] bool serialize(BlockTemplate& block, ISerializer& serializer);

[[nodiscard]] bool serialize(AccountPublicAddress& address, ISerializer& serializer);
[[nodiscard]] bool serialize(FeeAddress& addr, ISerializer& serializer);
[[nodiscard]] bool serialize(AccountKeys& keys, ISerializer& s);

[[nodiscard]] bool serialize(KeyPair& keyPair, ISerializer& serializer);
[[nodiscard]] bool serialize(RawBlock& rawBlock, ISerializer& serializer);
[[nodiscard]] bool serialize(FullBlock& block, ISerializer& serializer);
[[nodiscard]] bool serialize(LiteBlock& block, ISerializer& serializer);

}  // namespace CryptoNote
