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

#include "Xi/Crypto/FastHash.hpp"

namespace Xi {
namespace Crypto {

const FastHash FastHash::Null{};

Result<FastHash> FastHash::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  FastHash reval;
  if (FastHash::bytes() * 2 != hex.size()) {
    throw std::runtime_error{"wrong hex size"};
  }
  if (Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type)) != reval.size() * sizeof(value_type)) {
    throw std::runtime_error{"invalid hex string"};
  }
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Result<FastHash> FastHash::compute(Xi::ConstByteSpan data) {
  XI_ERROR_TRY();
  FastHash reval{};
  compute(data, reval).throwOnError();
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Result<void> FastHash::compute(Xi::ConstByteSpan data, FastHash &out) {
  XI_ERROR_TRY();
  Hash::fastHash(data, out.span());
  return Xi::success();
  XI_ERROR_CATCH();
}

Result<void> FastHash::compute(Xi::ConstByteSpan data, Xi::ByteSpan out) {
  XI_ERROR_TRY();
  Hash::fastHash(data, out);
  return Xi::success();
  XI_ERROR_CATCH();
}

Result<FastHash> FastHash::computeMerkleTree(Xi::ConstByteSpan data, size_t count) {
  XI_ERROR_TRY();
  FastHash reval{};
  computeMerkleTree(data, count, reval).throwOnError();
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Result<void> FastHash::computeMerkleTree(Xi::ConstByteSpan data, size_t count, FastHash &out) {
  XI_ERROR_TRY();
  Hash::treeHash(data, count, out.span());
  return Xi::success();
  XI_ERROR_CATCH();
}

Result<FastHash> FastHash::computeMerkleTree(ConstFastHashSpan data) {
  XI_ERROR_TRY();
  FastHash reval{};
  computeMerkleTree(data, reval).throwOnError();
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Result<void> FastHash::computeMerkleTree(ConstFastHashSpan data, FastHash &out) {
  XI_ERROR_TRY();
  Hash::treeHash(Xi::ConstByteSpan{reinterpret_cast<const Xi::Byte *>(data.data()), data.size() * FastHash::bytes()},
                 data.size(), out.span());
  return Xi::success();
  XI_ERROR_CATCH();
}

FastHash::FastHash() {
  nullify();
}

FastHash::FastHash(FastHash::array_type raw) : array_type(std::move(raw)) {
}

FastHash::~FastHash() {
}

bool FastHash::isNull() const {
  return *this == FastHash::Null;
}

std::string FastHash::toString() const {
  return Common::toHex(data(), size());
}

std::string FastHash::toShortString() const {
  std::string res = toString();
  res.erase(8, 48);
  res.insert(8, "....");
  return res;
}

Xi::ConstByteSpan FastHash::span() const {
  return Xi::ConstByteSpan{data(), bytes()};
}

Xi::ByteSpan FastHash::span() {
  return Xi::ByteSpan{data(), bytes()};
}

void FastHash::nullify() {
  fill(0);
}

}  // namespace Crypto

bool Crypto::serialize(FastHash &hash, Common::StringView name, CryptoNote::ISerializer &serializer) {
  return serializer.binary(hash.data(), FastHash::bytes(), name);
}

std::ostream &Crypto::operator<<(std::ostream &stream, const Xi::Crypto::FastHash &hash) {
  return stream << hash.toString();
}

}  // namespace Xi
