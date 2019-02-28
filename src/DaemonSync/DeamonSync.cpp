/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include <cinttypes>
#include <string>
#include <fstream>

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Global.h>
#include <Xi/Exceptional.h>
#include <Xi/Utils/Conversion.h>
#include <Xi/Utils/FileSystem.h>
#include <Xi/Config/Network.h>
#include <Xi/Config/Database.h>

#include <CommonCLI/CommonCLI.h>
#include <Logging/ConsoleLogger.h>
#include <Logging/LoggerRef.h>
#include <Common/Util.h>
#include <Common/StdInputStream.h>
#include <Common/StdOutputStream.h>
#include <Serialization/BinaryInputStreamSerializer.h>
#include <Serialization/BinaryOutputStreamSerializer.h>
#include <CryptoNoteCore/Core.h>
#include <CryptoNoteCore/CryptoNote.h>
#include <CryptoNoteCore/CryptoNoteSerialization.h>
#include <CryptoNoteCore/DataBaseConfig.h>
#include <CryptoNoteCore/RocksDBWrapper.h>
#include <CryptoNoteCore/MainChainStorage.h>
#include <CryptoNoteCore/DatabaseBlockchainCache.h>
#include <CryptoNoteCore/DatabaseBlockchainCacheFactory.h>

namespace {
// clang-format off
XI_DECLARE_EXCEPTIONAL_CATEGORY(Configuration)
XI_DECLARE_EXCEPTIONAL_INSTANCE(NoMethod, "You must specifiy whether you like to import or export.", Configuration)
XI_DECLARE_EXCEPTIONAL_INSTANCE(AmbigiousMethod, "You can either import or export but not both.", Configuration)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidDatabaseCompression, "Unknown database compression method given.", Configuration)
XI_DECLARE_EXCEPTIONAL_INSTANCE(ExportFileExists, "The file to export to already exists, please delete it or specify another.", Configuration)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidBatchSize, "Batch size must be at least 1.", Configuration)

XI_DECLARE_EXCEPTIONAL_CATEGORY(File)
XI_DECLARE_EXCEPTIONAL_INSTANCE(OpenFile, "Unable to open blockchain file.", File)
XI_DECLARE_EXCEPTIONAL_INSTANCE(UnsupportedVersion, "The file version is not supported.", File)

XI_DECLARE_EXCEPTIONAL_CATEGORY(Import)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidBlock, "Import file contains invalid blocks.", Import)
// clang-format on
}  // namespace

int main(int argc, char** argv) {
  using namespace Xi;
  using Logging::Level;

  try {
    cxxopts::Options cliOptions("xi-sync", "imports/exports the blockchain ");
    CommonCLI::emplaceCLIOptions(cliOptions);

    bool isImport = false;
    bool isExport = false;
    bool verbose = false;
    bool trunc = false;
    uint32_t batchSize = 1000;
    std::string dataDir = Tools::getDefaultDataDirectory();
    std::string file = "./xi-blockchain.dump";
    std::string checkpointsFile = "";
    Config::Network::Type network = Config::Network::defaultNetworkType();
    CryptoNote::DataBaseConfig::Compression dbCompression = CryptoNote::DataBaseConfig::Compression::LZ4;

    // clang-format off
    cliOptions.add_options("sync")
        ("i,import", "import the blockchain", cxxopts::value<bool>(isImport)->implicit_value("true"))
        ("e,export", "export the blockchain", cxxopts::value<bool>(isExport)->implicit_value("true"))
        ("v,verbose", "enables verbose logging", cxxopts::value<bool>(verbose)->implicit_value("true"))
        ("t,trunc", "forces to overwrite an exisitng file", cxxopts::value<bool>(trunc)->implicit_value("true"))
        ("b,batch-size", "number of blocks to process at once", cxxopts::value<uint32_t>(batchSize)->default_value(std::to_string(batchSize)))
        ("d,data-dir", "directory used to store the blockchain", cxxopts::value<std::string>(dataDir)->default_value(dataDir))
        ("c,checkpoints", "checkpoint file to speed up validation", cxxopts::value<std::string>(checkpointsFile))
        ("f,file", "file to read/write blockchain state to sync.", cxxopts::value<std::string>(file)->default_value(file))
        ("network", "The network type you want to connect to, mostly you want to use 'MainNet' here.",
            cxxopts::value<std::string>()->default_value(Xi::to_string(Xi::Config::Network::defaultNetworkType())),
            "[MainNet|StageNet|TestNet|LocalTestNet]")
        ("db-compression", "Compression to be used for storing the database.",
            cxxopts::value<std::string>()->default_value("lz4"), "[none|lz4|lz4hc]")
    ;
    // clang-format on
    auto cliParseResult = cliOptions.parse(argc, argv);
    if (CommonCLI::handleCLIOptions(cliOptions, cliParseResult)) {
      return 0;
    }

    XI_UNUSED_REVAL(CommonCLI::make_crash_dumper("xi-sync"));

    // ----------------------------------------- CLI Options Validation -----------------------------------------------
    if (!isImport && !isExport) {
      exceptional<NoMethodError>();
    } else if (isImport && isExport) {
      exceptional<AmbigiousMethodError>();
    }

    if (isExport && !trunc && FileSystem::exists(file).valueOrThrow()) {
      exceptional<ExportFileExistsError>();
    }

    if (batchSize < 1) {
      exceptional<InvalidBatchSizeError>();
    }

    network = lexical_cast<::Xi::Config::Network::Type>(cliParseResult["network"].as<std::string>());

    if (!CryptoNote::DataBaseConfig::parseCompression(cliParseResult["db-compression"].as<std::string>(),
                                                      dbCompression)) {
      exceptional<InvalidDatabaseCompressionError>();
    }
    // ----------------------------------------- CLI Options Validation -----------------------------------------------

    Logging::ConsoleLogger clogger{verbose ? Logging::TRACE : Logging::INFO};
    Logging::LoggerRef logger{clogger, "Sync"};
    auto currency = CryptoNote::CurrencyBuilder{clogger}.network(network).currency();

    CryptoNote::Checkpoints checkpoints{clogger};
    if (!checkpointsFile.empty()) {
      if (!checkpoints.loadCheckpointsFromFile(checkpointsFile)) {
        return -1;
      }
    } else {
      for (const auto& cp : Xi::Config::CHECKPOINTS) {
        checkpoints.addCheckpoint(cp.index, cp.blockId);
      }
    }
    logger(Level::INFO) << "loaded " << checkpoints.size() << " up to block index " << checkpoints.topCheckpointIndex();

    CryptoNote::DataBaseConfig dbConfig{/* */};
    dbConfig.init(dataDir, 1, 32, 32_MB, 128_MB);
    dbConfig.setNetwork(network);
    dbConfig.setCompression(dbCompression);
    FileSystem::ensureDirectoryExists(dbConfig.getDataDir()).throwOnError();

    CryptoNote::RocksDBWrapper database{clogger};
    database.init(dbConfig);

    if (!CryptoNote::DatabaseBlockchainCache::checkDBSchemeVersion(database, clogger)) {
      database.shutdown();
      // TODO flag it
      database.destoy(dbConfig);
      database.init(dbConfig);
    }

    System::Dispatcher dispatcher;
    CryptoNote::Core ccore{currency,
                           clogger,
                           std::move(checkpoints),
                           dispatcher,
                           std::make_unique<CryptoNote::DatabaseBlockchainCacheFactory>(database, clogger),
                           CryptoNote::createSwappedMainChainStorage(dataDir, currency)};

    ccore.load();

    logger(Level::INFO) << "local database height: " << (ccore.getTopBlockIndex() + 1);

    const uint8_t supportedVersion = 1;
    uint8_t fileVersion = supportedVersion;
    if (isImport) {
      std::ifstream input{file, std::ios::in | std::ios::binary};
      if (!input.good()) {
        exceptional<OpenFileError>();
      }
      Common::StdInputStream inputStream{input};
      CryptoNote::BinaryInputStreamSerializer serializer{inputStream};
      serializer(fileVersion, "version");

      if (fileVersion != supportedVersion) {
        exceptional<UnsupportedVersionError>();
      }

      uint32_t readBlocks = 0;
      while (input.peek() != EOF) {
        const uint32_t currentTopIndex = ccore.getTopBlockIndex();
        std::vector<CryptoNote::RawBlock> blocks;
        blocks.reserve(batchSize);
        for (size_t i = 0; i < batchSize && input.peek() != EOF; ++i) {
          CryptoNote::RawBlock iBlock{/* */};
          CryptoNote::serialize(iBlock, serializer);
          blocks.emplace_back(std::move(iBlock));
        }

        for (auto&& iBlock : blocks) {
          if (readBlocks++ <= ccore.getTopBlockIndex()) {
            continue;
          }
          auto pushResult = ccore.addBlock(std::move(iBlock));
          if (pushResult != CryptoNote::error::AddBlockErrorCondition::BLOCK_ADDED) {
            logger(Level::FATAL) << "Unable to add block: " << pushResult.message();
            exceptional<InvalidBlockError>();
          }
        }

        const uint32_t newTopBlockIndex = ccore.getTopBlockIndex();
        if (currentTopIndex < newTopBlockIndex) {
          logger(Level::INFO) << "updated height: " << (ccore.getTopBlockIndex() + 1);
        }
      }
    } else {
      std::ofstream output{file, std::ios::out | std::ios::trunc | std::ios::binary};
      if (!output.good()) {
        exceptional<OpenFileError>();
      }
      Common::StdOutputStream outputStream{output};
      CryptoNote::BinaryOutputStreamSerializer serializer{outputStream};

      serializer(fileVersion, "version");

      uint32_t currentBlockIndex = 0;
      while (currentBlockIndex <= ccore.getTopBlockIndex()) {
        auto blocks = ccore.getBlocks(currentBlockIndex, batchSize);
        for (auto& iBlock : blocks) {
          CryptoNote::serialize(iBlock, serializer);
        }
        currentBlockIndex += static_cast<uint32_t>(blocks.size());

        logger(Level::INFO) << "written blocks " << currentBlockIndex << " of " << (ccore.getTopBlockIndex() + 1);
      }
    }

    ccore.save();
    database.shutdown();

  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  return 0;
}
