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

#include <string>

#include <Serialization/ISerializer.h>

#include "IMainChainStorage.h"
#include "SwappedVector.h"
#include "Currency.h"

namespace CryptoNote {

class MainChainStorage : public IMainChainStorage {
 public:
  struct Entity {
    RawBlock block;
    uint64_t blobSize;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER_RENAME(block, block)
    KV_MEMBER_RENAME(blobSize, blob_size)
    KV_END_SERIALIZATION
  };

 public:
  MainChainStorage(const std::string& blocksFilame, const std::string& indexesFilename);
  ~MainChainStorage() override;

  virtual void pushBlock(const RawBlock& rawBlock, const uint64_t blobSize) override;
  virtual void popBlock() override;

  virtual RawBlock getBlockByIndex(uint32_t index) const override;
  virtual uint64_t getBlobSizeByIndex(uint32_t index) const override;
  virtual uint32_t getBlockCount() const override;

  virtual void clear() override;

 private:
  mutable SwappedVector<Entity> storage;
};

std::unique_ptr<IMainChainStorage> createSwappedMainChainStorage(const std::string& dataDir, const Currency& currency);

}  // namespace CryptoNote
