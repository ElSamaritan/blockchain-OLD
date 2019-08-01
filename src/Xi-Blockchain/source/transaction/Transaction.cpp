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

#include "Xi/Blockchain/Transaction/Transaction.hpp"

#include <numeric>

#include <Xi/Exceptions.hpp>

namespace Xi {
namespace Blockchain {
namespace Transaction {

const Transaction Transaction::Null{};

Transaction::Transaction() : Prefix() {
  nullify();
}

Transaction::~Transaction() {
  /* */
}

Hash Transaction::hash() const {
  if (!signatures.has_value()) {
    return prefixHash();
  } else if (auto pruned = std::get_if<Pruned>(std::addressof(*signatures))) {
    std::array<Hash, 2> hashes{};
    hashes[0] = prefixHash();
    hashes[1] = pruned->hash;
    return Hash::computeMerkleTree(hashes).takeOrThrow();
  } else if (auto raw = std::get_if<SignatureVector>(std::addressof(*signatures))) {
    if (raw->empty()) {
      return prefixHash();
    } else {
      std::array<Hash, 2> hashes{};
      hashes[0] = prefixHash();
      hashes[1] = Hash::computeObjectHash(*raw).takeOrThrow();
      return Hash::computeMerkleTree(hashes).takeOrThrow();
    }
  } else {
    exceptional<InvalidVariantTypeError>("invalid variant type for transaction signatures");
  }
}

uint64_t Transaction::binarySize() const {
  return prefixBinarySize() + signaturesBinarySize();
}

uint64_t Transaction::signaturesBinarySize() const {
  if (!signatures.has_value()) {
    return 0ULL;
  } else if (auto pruned = std::get_if<Pruned>(std::addressof(*signatures))) {
    return pruned->binarySize;
  } else if (auto raw = std::get_if<SignatureVector>(std::addressof(*signatures))) {
    if (raw->empty()) {
      return 0ULL;
    } else {
      return std::accumulate(raw->begin(), raw->end(), 0ULL, [](const auto acc, const auto& vec) {
        if (const auto ringSig = std::get_if<RingSignature>(std::addressof(vec))) {
          return acc + ringSig->size() * Crypto::Signature::bytes();
        } else {
          exceptional<InvalidVariantTypeError>("invalid variant type for transaction signature");
        }
      });
    }
  } else {
    exceptional<InvalidVariantTypeError>("invalid variant type for transaction signatures");
  }
}

bool Transaction::isNull() const {
  return version == 0;
}

void Transaction::nullify() {
  version = 0;
  unlockTime = 0;
  inputs.clear();
  outputs.clear();
  extra.clear();
  signatures = SignatureVector{};
}

void Transaction::prune() {
  if (!signatures.has_value()) {
    /* no need to prune */
  }
  if (auto pruned = std::get_if<Pruned>(std::addressof(*signatures))) {
    XI_UNUSED(pruned);  // already pruned
  } else if (auto raw = std::get_if<SignatureVector>(std::addressof(*signatures))) {
    if (!raw->empty()) {
      signatures = Pruned{Hash::computeObjectHash(*raw).takeOrThrow(), signaturesBinarySize()};
    }
  } else {
    exceptional<InvalidVariantTypeError>("invalid variant type for transaction signatures");
  }
}

bool Transaction::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(Prefix::serialize(serializer), false);
  if (version == 0) {
    if (serializer.isInput()) {
      nullify();
      return true;
    } else {
      assert(serializer.isOutput());
      XI_RETURN_EC_IF_NOT(unlockTime == 0, false);
      XI_RETURN_EC_IF_NOT(extra.empty(), false);
      XI_RETURN_EC_IF(signatures.has_value(), false);
      XI_RETURN_EC_IF_NOT(inputs.empty(), false);
      XI_RETURN_EC_IF_NOT(outputs.empty(), false);
      return true;
    }
  }
  XI_RETURN_EC_IF(inputs.empty(), false);
  if (std::holds_alternative<RewardInput>(inputs.front())) {
    if (serializer.isOutput()) {
      XI_RETURN_EC_IF(signatures.has_value(), false);
    } else {
      signatures = std::nullopt;
    }
    return true;
  }

  // TODO
  XI_RETURN_EC_IF_NOT(serializer(signatures, "signatures"), false);
  //  XI_RETURN_EC_IF_NOT(signatures.has_value(), false);
  //  bool isPruned = std::holds_alternative<Pruned>(*signatures);
  //  XI_RETURN_EC_IF_NOT(serializer(isPruned, "pruned"), false);
  //  if (serializer.isInput()) {
  //    if (isPruned) {
  //      signatures = Pruned{};
  //    } else {
  //      signatures = SignatureCollection{};
  //    }
  //  }

  //  if (auto pruned = std::get_if<TransactionSignaturePruned>(std::addressof(signatures))) {
  //    XI_RETURN_EC_IF_NOT(serializer(*pruned, "signatures"), false);
  //  } else if (auto raw = std::get_if<TransactionSignatureCollection>(std::addressof(signatures))) {
  //    auto& signatures = *raw;
  //    const size_t outerSignaturesCount = inputs.size();
  //    XI_RETURN_EC_IF_NOT(serializer.beginStaticArray(outerSignaturesCount, "signatures"), false);

  //    XI_RETURN_EC_IF_NOT(outerSignaturesCount == inputs.size(), false);
  //    if (serializer.isInput()) {
  //      signatures.resize(outerSignaturesCount, {});
  //    }
  //    for (size_t i = 0; i < outerSignaturesCount; ++i) {
  //      XI_RETURN_EC_IF_NOT(std::holds_alternative<KeyInput>(inputs[i]), false);
  //      const auto& iInput = std::get<KeyInput>(inputs[i]);
  //      const size_t innerSignaturesCount = iInput.outputIndices.size();
  //      XI_RETURN_EC_IF_NOT(serializer.beginStaticArray(innerSignaturesCount, ""), false);
  //      if (serializer.isInput()) {
  //        signatures[i].resize(innerSignaturesCount, Crypto::Signature::Null);
  //      } else if (serializer.isOutput()) {
  //        XI_RETURN_EC_IF_NOT(signatures[i].size() == innerSignaturesCount, false);
  //      }
  //      for (size_t j = 0; j < innerSignaturesCount; ++j) {
  //        XI_RETURN_EC_IF_NOT(serializer(signatures[i][j], ""), false);
  //      }
  //      XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  //    }

  //    XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  //  } else {
  //    XI_RETURN_EC(false);
  //  }

  XI_RETURN_SC(true);
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
