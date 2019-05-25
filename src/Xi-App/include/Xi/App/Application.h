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

#pragma once

#include <memory>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>
#include <Xi/Http/SSLConfiguration.h>
#include <System/Dispatcher.h>
#include <Logging/LoggerManager.h>
#include <CryptoNoteCore/RocksDBWrapper.h>
#include <CryptoNoteCore/Checkpoints.h>
#include <CryptoNoteCore/ICore.h>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/INode.h>
#include <Rpc/RpcRemoteConfiguration.h>

#include "Xi/App/LoggingOptions.h"
#include "Xi/App/DatabaseOptions.h"
#include "Xi/App/RemoteRpcOptions.h"
#include "Xi/App/NetworkOptions.h"
#include "Xi/App/CheckpointsOptions.h"

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
  virtual ~Application() = default;

  int exec(int argc, char** argv);

  const std::string name() const;
  const std::string description() const;
  Logging::LoggerManager& logger();
  System::Dispatcher& dispatcher();

  CryptoNote::RpcRemoteConfiguration remoteConfiguration() const;

  // --------------------------------------------- PreSetup ---------------------------------------------------
  CryptoNote::RocksDBWrapper* database();
  CryptoNote::Checkpoints* checkpoints();
  CryptoNote::Currency* currency();
  CryptoNote::ICore* core();
  CryptoNote::INode* remoteNode(bool pollUpdates = false);
  // --------------------------------------------- PreSetup ---------------------------------------------------

  // --------------------------------------------- Overrides --------------------------------------------------
 protected:
  virtual void makeOptions(cxxopts::Options& options);
  virtual bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result);
  virtual void setUp();
  virtual void tearDown();
  virtual int run() = 0;
  // --------------------------------------------- Overrides --------------------------------------------------

  // ---------------------------------------------- Helper ----------------------------------------------------
 protected:
  // TODO: Move to module interface and require modules definied by the library implementing the feature.
  void useLogging(Logging::Level defaultLevel = Logging::INFO);
  void useDatabase();
  void useRemoteRpc();
  void useNetwork();
  void useCurrency();
  void useCheckpoints();
  void useCore();
  // ---------------------------------------------- Helper ----------------------------------------------------

 private:
  bool isSSLClientRequired() const;
  bool isSSLServerRequired() const;

  void initializeLogger();
  void initializeDatabase();
  void initializeCheckpoints();
  void initializeCurrency();
  void initializeCore();

 private:
  const std::string m_name;
  const std::string m_description;
  System::Dispatcher m_dispatcher;

  // ------------------------------------------------ Logging ------------------------------------------------------
  std::unique_ptr<Logging::LoggerManager> m_logger;
  std::unique_ptr<Logging::LoggerRef> m_ologger;
  std::unique_ptr<Logging::ILogger> m_consoleLogger;
  std::unique_ptr<Logging::ILogger> m_fileLogger;
  std::unique_ptr<LoggingOptions> m_logOptions;
  // ------------------------------------------------ Logging ------------------------------------------------------

  std::unique_ptr<DatabaseOptions> m_dbOptions;
  std::unique_ptr<RemoteRpcOptions> m_remoteRpcOptions;
  std::unique_ptr<NetworkOptions> m_netOptions;
  std::unique_ptr<CheckpointsOptions> m_checkpointOptions;
  Xi::Http::SSLConfiguration m_sslConfig;
  bool m_coreRequied = false;
  bool m_currencyRequired = false;

  std::unique_ptr<CryptoNote::RocksDBWrapper> m_database;
  std::unique_ptr<CryptoNote::Checkpoints> m_checkpoints;
  std::unique_ptr<CryptoNote::ICore> m_core;
  std::unique_ptr<CryptoNote::Currency> m_currency;
  std::unique_ptr<CryptoNote::INode> m_remoteNode;
};
}  // namespace App
}  // namespace Xi

#define XI_DECLARE_APP(APP) \
  std::unique_ptr<Xi::App::Application> Xi::App::Application::makeInstance() { return std::make_unique<APP>(); }

#define XI_APP() Xi::App::Application::instance()
