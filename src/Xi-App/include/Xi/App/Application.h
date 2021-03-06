﻿/* ============================================================================================== *
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

#pragma once

#include <memory>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>
#include <Xi/Http/SSLConfiguration.h>
#include <Xi/Http/Server.h>
#include <System/Dispatcher.h>
#include <Logging/LoggerManager.h>
#include <Logging/ConsoleLogger.h>
#include <CryptoNoteCore/RocksDBWrapper.h>
#include <CryptoNoteCore/Checkpoints.h>
#include <CryptoNoteCore/ICore.h>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/INode.h>
#include <P2p/NetNode.h>
#include <Rpc/RpcRemoteConfiguration.h>
#include <Rpc/RpcServer.h>

#include "Xi/App/GeneralOptions.h"
#include "Xi/App/LicenseOptions.h"
#include "Xi/App/BreakpadOptions.h"
#include "Xi/App/LoggingOptions.h"
#include "Xi/App/DatabaseOptions.h"
#include "Xi/App/RemoteRpcOptions.h"
#include "Xi/App/NetworkOptions.h"
#include "Xi/App/CheckpointsOptions.h"
#include "Xi/App/NodeOptions.h"
#include "Xi/App/PublicNodeOptions.h"
#include "Xi/App/BlockExplorerOptions.h"
#include "Xi/App/ServerRpcOptions.h"
#include "Xi/App/SslOptions.h"
#include "Xi/App/SslClientOptions.h"
#include "Xi/App/SslServerOptions.h"

namespace Xi {
namespace App {
class Application {
 public:
  static Application& instance();

 protected:
  Application(std::string name, std::string description);

 public:
  static std::unique_ptr<Application> makeInstance();

 public:
  virtual ~Application();

  int exec(int argc, char** argv);

  const std::string name() const;
  const std::string description() const;
  Logging::LoggerManager& logger();
  Logging::ConsoleLogger& consoleLogger();
  System::Dispatcher& dispatcher();

  CryptoNote::RpcRemoteConfiguration remoteConfiguration();

  // --------------------------------------------- PreSetup ---------------------------------------------------
  CryptoNote::RocksDBWrapper* database();
  std::string dataDirectory();
  CryptoNote::Checkpoints* checkpoints();
  CryptoNote::CurrencyBuilder* intermediateCurrency();
  CryptoNote::Currency* currency();
  CryptoNote::ICore* core();
  CryptoNote::INode* rpcNode(bool pollUpdates = false, bool preferEmbbedded = false);
  CryptoNote::NodeServer* node();
  Http::SSLConfiguration* ssl();
  CryptoNote::RpcServer* rpcServer();
  CryptoNote::CryptoNoteProtocolHandler* protocol();
  // --------------------------------------------- PreSetup ---------------------------------------------------

  // --------------------------------------------- Overrides --------------------------------------------------
 protected:
  virtual void loadEnvironment(Environment& env);
  virtual void makeOptions(cxxopts::Options& options);
  virtual bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result);
  virtual void setUp();
  virtual void tearDown();
  virtual int run() = 0;
  // --------------------------------------------- Overrides --------------------------------------------------

  // ---------------------------------------------- Helper ----------------------------------------------------
 protected:
  // TODO: Move to module interface and require modules definied by the library implementing the feature.
  void useLogging(Logging::Level defaultLevel = Logging::Warning);
  void useDatabase();
  void useRemoteRpc();
  void useNetwork();
  void useCurrency();
  void useCheckpoints();
  void useCore();
  void useNode();
  void useRpcServer();
  void useSsl();
  void useSslServer();
  void useSslClient();
  // ---------------------------------------------- Helper ----------------------------------------------------

 private:
  void initializeLogger();
  void initializeDatabase();
  void initializeCheckpoints();
  void initializeIntermediateCurrency();
  void initializeCurrency();
  void initializeCore();
  void initializeNode();
  void initializeSsl();
  void initializeRpcServer();

 private:
  const std::string m_name;
  const std::string m_description;
  System::Dispatcher m_dispatcher;

  // ------------------------------------------------ Logging ------------------------------------------------------
  std::unique_ptr<Logging::LoggerManager> m_logger;
  std::unique_ptr<Logging::LoggerRef> m_ologger;
  std::unique_ptr<Logging::ConsoleLogger> m_consoleLogger;
  std::unique_ptr<Logging::ILogger> m_fileLogger;
  std::unique_ptr<Logging::ILogger> m_discordLogger;
  std::unique_ptr<LoggingOptions> m_logOptions;
  // ------------------------------------------------ Logging ------------------------------------------------------

  GeneralOptions m_generalOptions{};
  LicenseOptions m_licenseOptions{};
  BreakpadOptions m_breakpadOptions{};

  std::unique_ptr<DatabaseOptions> m_dbOptions;
  std::unique_ptr<RemoteRpcOptions> m_remoteRpcOptions;
  std::unique_ptr<NetworkOptions> m_netOptions;
  std::unique_ptr<CheckpointsOptions> m_checkpointOptions;
  std::unique_ptr<NodeOptions> m_nodeOptions;
  std::unique_ptr<ServerRpcOptions> m_rpcServerOptions;
  std::unique_ptr<PublicNodeOptions> m_publicNodeOptions;
  std::unique_ptr<BlockExplorerOptions> m_blockExplorerOptions;
  std::unique_ptr<SslOptions> m_sslOptions;
  std::unique_ptr<SslClientOptions> m_sslClientOptions;
  std::unique_ptr<SslServerOptions> m_sslServerOptions;
  bool m_coreRequied = false;
  bool m_currencyRequired = false;

  std::unique_ptr<CryptoNote::RocksDBWrapper> m_database;
  std::unique_ptr<CryptoNote::Checkpoints> m_checkpoints;
  std::unique_ptr<CryptoNote::ICore> m_core;
  std::unique_ptr<CryptoNote::Currency> m_currency;
  std::unique_ptr<CryptoNote::CurrencyBuilder> m_intermediateCurrency;
  std::unique_ptr<CryptoNote::INode> m_remoteNode;
  std::unique_ptr<CryptoNote::NodeServer> m_node;
  std::unique_ptr<CryptoNote::CryptoNoteProtocolHandler> m_protocol;
  std::shared_ptr<CryptoNote::RpcServer> m_rpcServer;
  std::unique_ptr<Http::SSLConfiguration> m_sslConfig;
};
}  // namespace App
}  // namespace Xi

#define XI_DECLARE_APP(APP)                                                    \
  std::unique_ptr<Xi::App::Application> Xi::App::Application::makeInstance() { \
    return std::make_unique<APP>();                                            \
  }

#define XI_APP() Xi::App::Application::instance()
