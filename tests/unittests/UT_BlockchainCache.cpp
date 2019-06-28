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

#include <gtest/gtest.h>

#include <memory>
#include <string>

#include <Xi/FileSystem.h>
#include <Logging/ConsoleLogger.h>
#include <crypto/crypto.h>
#include <CryptoNoteCore/BlockchainCache.h>

namespace {

class CryptoNote_BlockchainCache : public ::testing::Test {
 public:
  std::string filename{"./blockchain_cache_test"};
  Logging::ConsoleLogger logger{Logging::TRACE};
  std::unique_ptr<CryptoNote::Currency> currency;
  std::unique_ptr<CryptoNote::BlockchainCache> cache;

  void SetUp() override {
    using namespace CryptoNote;

    Xi::FileSystem::removeFileIfExists(filename).throwOnError();
    currency = std::make_unique<Currency>(CurrencyBuilder{logger}.currency());
    cache = std::make_unique<BlockchainCache>(filename, *currency, logger, nullptr);
    ASSERT_NE(cache.get(), nullptr);
  }

  void TearDown() override {
    cache.release();
    ASSERT_EQ(cache, nullptr);
  }
};

}  // namespace

TEST_F(CryptoNote_BlockchainCache, TransactionHashConsistency) {
  using namespace CryptoNote;
  using namespace Xi::Crypto::Hash;

  CachedBlock genesisBlock{currency->genesisBlock()};
  CachedTransaction genesisTransaction{cache->getRawTransaction(0, 0)};
  CachedTransaction staticRewardTransaction{cache->getRawTransaction(0, 1)};
  const auto genesisHash = cache->getTopBlockHash();

  EXPECT_EQ(genesisHash.toString(), genesisBlock.getBlockHash().toString());
  EXPECT_EQ(genesisTransaction.getTransactionHash().toString(),
            CachedTransaction{genesisBlock.getBlock().baseTransaction}.getTransactionHash().toString());
  EXPECT_EQ(crc16(staticRewardTransaction.getTransactionHash().span()).toString(),
            genesisBlock.getBlock().staticRewardHash->toString());
}
