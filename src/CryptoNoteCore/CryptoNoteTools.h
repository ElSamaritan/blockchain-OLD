﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#include <limits>
#include <vector>

#include "Common/MemoryInputStream.h"
#include "Common/StringTools.h"
#include "Common/VectorOutputStream.h"
#include "Serialization/BinaryOutputStreamSerializer.h"
#include "Serialization/BinaryInputStreamSerializer.h"
#include <Xi/Config.h>
#include "CryptoNoteSerialization.h"

namespace CryptoNote {

void getBinaryArrayHash(const BinaryArray& binaryArray, Crypto::Hash& hash);
Crypto::Hash getBinaryArrayHash(const BinaryArray& binaryArray);

// noexcept
template <class T>
bool toBinaryArray(const T& object, BinaryArray& binaryArray) {
  try {
    binaryArray = toBinaryArray(object);
  } catch (std::exception&) {
    return false;
  }

  return true;
}

template <>
bool toBinaryArray(const BinaryArray& object, BinaryArray& binaryArray);

// throws exception if serialization failed
template <class T>
BinaryArray toBinaryArray(const T& object) {
  BinaryArray ba;
  ::Common::VectorOutputStream stream(ba);
  BinaryOutputStreamSerializer serializer(stream);
  serialize(const_cast<T&>(object), serializer);
  return ba;
}

template <class T>
T fromBinaryArray(const BinaryArray& binaryArray) {
  T object;
  Common::MemoryInputStream stream(binaryArray.data(), binaryArray.size());
  BinaryInputStreamSerializer serializer(stream);
  serialize(object, serializer);
  if (!stream.endOfStream()) {  // check that all data was consumed
    throw std::runtime_error("failed to unpack type");
  }

  return object;
}

template <class T>
bool fromBinaryArray(T& object, const BinaryArray& binaryArray) {
  try {
    object = fromBinaryArray<T>(binaryArray);
  } catch (std::exception&) {
    return false;
  }

  return true;
}

template <class T>
bool getObjectBinarySize(const T& object, size_t& size) {
  BinaryArray ba;
  if (!toBinaryArray(object, ba)) {
    size = (std::numeric_limits<size_t>::max)();
    return false;
  }

  size = ba.size();
  return true;
}

template <class T>
size_t getObjectBinarySize(const T& object) {
  size_t size;
  getObjectBinarySize(object, size);
  return size;
}

template <class T>
bool getObjectHash(const T& object, Crypto::Hash& hash) {
  BinaryArray ba;
  if (!toBinaryArray(object, ba)) {
    hash = NULL_HASH;
    return false;
  }

  hash = getBinaryArrayHash(ba);
  return true;
}

template <class T>
bool getObjectHash(const T& object, Crypto::Hash& hash, size_t& size) {
  BinaryArray ba;
  if (!toBinaryArray(object, ba)) {
    hash = NULL_HASH;
    size = (std::numeric_limits<size_t>::max)();
    return false;
  }

  size = ba.size();
  hash = getBinaryArrayHash(ba);
  return true;
}

template <class T>
Crypto::Hash getObjectHash(const T& object) {
  Crypto::Hash hash;
  getObjectHash(object, hash);
  return hash;
}

inline bool getBaseTransactionHash(const BaseTransaction& tx, Crypto::Hash& hash) { return getObjectHash(tx, hash); }

uint64_t getInputAmount(const Transaction& transaction);
std::vector<uint64_t> getInputsAmounts(const Transaction& transaction);
uint64_t getOutputAmount(const Transaction& transaction);
void decomposeAmount(uint64_t amount, uint64_t dustThreshold, std::vector<uint64_t>& decomposedAmounts);

bool isCanonicalAmount(uint64_t amount);

size_t countCanonicalDecomposition(const Transaction& tx);
size_t countCanonicalDecomposition(const std::vector<uint64_t>& amounts);
}  // namespace CryptoNote
