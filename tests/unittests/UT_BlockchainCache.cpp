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
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/BlockchainCache.h>
#include <CryptoNoteCore/RocksDBWrapper.h>
#include <CryptoNoteCore/DatabaseBlockchainCache.h>
#include <CryptoNoteCore/DatabaseBlockchainCacheFactory.h>

namespace {

class CryptoNote_BlockchainCache : public ::testing::Test {
 public:
  std::string filename{"./blockchain_cache_test"};
  Logging::ConsoleLogger logger{Logging::Trace};
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

class CryptoNote_DatabaseBlockchainCache : public ::testing::Test {
 public:
  std::string dir{"./dbblockchain_cache_test"};
  Logging::ConsoleLogger logger{Logging::Error};
  std::unique_ptr<CryptoNote::Currency> currency;
  std::unique_ptr<CryptoNote::DatabaseBlockchainCacheFactory> factory;
  std::unique_ptr<CryptoNote::RocksDBWrapper> database;
  std::unique_ptr<CryptoNote::DatabaseBlockchainCache> cache;

  void SetUp() override {
    using namespace CryptoNote;

    Xi::FileSystem::removeDircetoryIfExists(dir).throwOnError();
    Xi::FileSystem::ensureDirectoryExists(dir).throwOnError();
    currency = std::make_unique<Currency>(CurrencyBuilder{logger}.currency());
    CryptoNote::DataBaseConfig config{};
    config.setDataDir(dir);
    database = std::make_unique<CryptoNote::RocksDBWrapper>(logger);
    database->init(config);
    factory = std::make_unique<CryptoNote::DatabaseBlockchainCacheFactory>(*database, logger);
    cache = std::make_unique<CryptoNote::DatabaseBlockchainCache>(*currency, *database, *factory, logger);
    ASSERT_NE(cache.get(), nullptr);
  }

  void TearDown() override {
    cache.release();
    factory.release();
    database->shutdown();
    database.release();
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

TEST_F(CryptoNote_DatabaseBlockchainCache, TransactionHashConsistency) {
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

TEST_F(CryptoNote_DatabaseBlockchainCache, CachedBlockInfo) {
  using namespace CryptoNote;
  using namespace Xi::Crypto::Hash;

  auto height = BlockHeight::Genesis;
  CachedBlockInfo genesis = cache->getBlockInfos(Xi::makeSpan(height)).front();

  CachedBlockInfo pushed;

  BlockTemplate tmp;
  tmp.timestamp = genesis.timestamp + 60;
  tmp.previousBlockHash = genesis.blockHash;
  tmp.version = genesis.version;
  tmp.upgradeVote = genesis.upgradeVote;
  tmp.baseTransaction.version = 1;
  tmp.baseTransaction.inputs.push_back(BaseInput{height.next(1)});
  TransactionValidatorState vstate{};
  RawBlock raw;
  raw.blockTemplate = toBinaryArray(tmp);
  cache->pushBlock(CachedBlock{tmp}, {}, vstate, tmp.baseTransaction.binarySize(), 100, 200, std::move(raw));

  CachedBlockInfo genesis2 = cache->getBlockInfos(Xi::makeSpan(height)).front();
  auto pushedHeight = height.next(1);
  CachedBlockInfo pushedNfo = cache->getBlockInfos(Xi::makeSpan(pushedHeight)).front();

  EXPECT_EQ(genesis.blockHash.toString(), genesis2.blockHash.toString());
  EXPECT_EQ(genesis.version, genesis2.version);
  EXPECT_EQ(genesis.upgradeVote, genesis2.upgradeVote);
  EXPECT_EQ(genesis.timestamp, genesis2.timestamp);
  EXPECT_EQ(genesis.blobSize, genesis2.blobSize);
  EXPECT_EQ(genesis.cumulativeDifficulty, genesis2.cumulativeDifficulty);
  EXPECT_EQ(genesis.alreadyGeneratedCoins, genesis2.alreadyGeneratedCoins);
  EXPECT_EQ(genesis.alreadyGeneratedTransactions, genesis2.alreadyGeneratedTransactions);
}
