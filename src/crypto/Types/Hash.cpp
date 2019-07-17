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

#include <stdexcept>
#include <utility>

#include <Xi/Crypto/Hash/FastHash.hh>
#include <Xi/Crypto/Hash/TreeHash.hh>
#include <Common/StringTools.h>

#include "crypto/Types/Hash.h"

const Crypto::Hash Crypto::Hash::Null{};

Xi::Result<Crypto::Hash> Crypto::Hash::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  Hash reval;
  if (Hash::bytes() * 2 != hex.size()) {
    throw std::runtime_error{"wrong hex size"};
  }
  if (Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type)) != reval.size() * sizeof(value_type)) {
    throw std::runtime_error{"invalid hex string"};
  }
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Xi::Result<Crypto::Hash> Crypto::Hash::compute(Xi::ConstByteSpan data) {
  XI_ERROR_TRY();
  Hash reval{};
  compute(data, reval).throwOnError();
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Xi::Result<void> Crypto::Hash::compute(Xi::ConstByteSpan data, Crypto::Hash &out) {
  XI_ERROR_TRY();
  Xi::Crypto::Hash::fastHash(data, out.span());
  return Xi::success();
  XI_ERROR_CATCH();
}

Xi::Result<void> Crypto::Hash::compute(Xi::ConstByteSpan data, Xi::ByteSpan out) {
  XI_ERROR_TRY();
  Xi::Crypto::Hash::fastHash(data, out);
  return Xi::success();
  XI_ERROR_CATCH();
}

Xi::Result<Crypto::Hash> Crypto::Hash::computeMerkleTree(Xi::ConstByteSpan data, size_t count) {
  XI_ERROR_TRY();
  Hash reval{};
  computeMerkleTree(data, count, reval).throwOnError();
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Xi::Result<void> Crypto::Hash::computeMerkleTree(Xi::ConstByteSpan data, size_t count, Crypto::Hash &out) {
  XI_ERROR_TRY();
  Xi::Crypto::Hash::treeHash(data, count, out.span());
  return Xi::success();
  XI_ERROR_CATCH();
}

Xi::Result<Crypto::Hash> Crypto::Hash::computeMerkleTree(ConstHashSpan data) {
  XI_ERROR_TRY();
  Hash reval{};
  computeMerkleTree(data, reval).throwOnError();
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Xi::Result<void> Crypto::Hash::computeMerkleTree(ConstHashSpan data, Crypto::Hash &out) {
  XI_ERROR_TRY();
  Xi::Crypto::Hash::treeHash(
      Xi::ConstByteSpan{reinterpret_cast<const Xi::Byte *>(data.data()), data.size() * Crypto::Hash::bytes()},
      data.size(), out.span());
  return Xi::success();
  XI_ERROR_CATCH();
}

Crypto::Hash::Hash() {
  nullify();
}

Crypto::Hash::Hash(Crypto::Hash::array_type raw) : array_type(std::move(raw)) {
}

Crypto::Hash::~Hash() {
}

bool Crypto::Hash::isNull() const {
  return *this == Hash::Null;
}

std::string Crypto::Hash::toString() const {
  return Common::toHex(data(), size());
}

std::string Crypto::Hash::toShortString() const {
  std::string res = toString();
  res.erase(8, 48);
  res.insert(8, "....");
  return res;
}

Xi::ConstByteSpan Crypto::Hash::span() const {
  return Xi::ConstByteSpan{data(), bytes()};
}

Xi::ByteSpan Crypto::Hash::span() {
  return Xi::ByteSpan{data(), bytes()};
}

void Crypto::Hash::nullify() {
  fill(0);
}

bool Crypto::serialize(Crypto::Hash &hash, Common::StringView name, CryptoNote::ISerializer &serializer) {
  return serializer.binary(hash.data(), Hash::bytes(), name);
}
