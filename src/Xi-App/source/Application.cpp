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

#include "Xi/App/Application.h"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <cassert>

#include <Xi/Exceptional.hpp>
#include <CommonCLI/CommonCLI.h>
#include <Logging/ConsoleLogger.h>
#include <Logging/FileLogger.h>
#include <CryptoNoteCore/Core.h>
#include <CryptoNoteCore/DatabaseBlockchainCache.h>
#include <CryptoNoteCore/DatabaseBlockchainCacheFactory.h>
#include <CryptoNoteCore/MainChainStorage.h>
#include <NodeRpcProxy/NodeRpcProxy.h>

namespace {
// clang-format off
XI_DECLARE_EXCEPTIONAL_CATEGORY(ApplicationSetup)
XI_DECLARE_EXCEPTIONAL_INSTANCE(MissingDependency, "a module could not be set up, due to a missing dependency setup", ApplicationSetup);
XI_DECLARE_EXCEPTIONAL_INSTANCE(ModuleSetUp, "a module failed to initialize", ApplicationSetup)
// clang-format on
}  // namespace

Xi::App::Application &Xi::App::Application::instance() {
  static std::unique_ptr<Application> __Instance = makeInstance();
  return *__Instance;
}

Xi::App::Application::Application(std::string _name, std::string _description)
    : m_name{std::move(_name)}, m_description{std::move(_description)} {
  m_logger = std::make_unique<Logging::LoggerManager>();
  m_ologger = std::make_unique<Logging::LoggerRef>(*m_logger, "App");
}

int Xi::App::Application::exec(int argc, char **argv) {
  XI_UNUSED(argc, argv);
  try {
    cxxopts::Options cliOptions{name(), description()};
    makeOptions(cliOptions);
    const auto parseResult = cliOptions.parse(argc, argv);
    if (evaluateParsedOptions(cliOptions, parseResult)) {
      return 0;
    }
    XI_UNUSED_REVAL(CommonCLI::make_crash_dumper(name()));
    setUp();
    int returnCode = run();
    tearDown();
    return returnCode;
  } catch (cxxopts::OptionParseException &e) {
    std::cerr << "error parsing command line options: " << e.what() << std::endl;
    return -1;
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "unknown error in application exec routine." << std::endl;
    return -1;
  }
}

const std::string Xi::App::Application::name() const { return m_name; }

const std::string Xi::App::Application::description() const { return m_description; }

Logging::LoggerManager &Xi::App::Application::logger() { return *m_logger; }

System::Dispatcher &Xi::App::Application::dispatcher() { return m_dispatcher; }

CryptoNote::RpcRemoteConfiguration Xi::App::Application::remoteConfiguration() const {
  return m_remoteRpcOptions->getConfig(m_sslConfig);
}

CryptoNote::RocksDBWrapper *Xi::App::Application::database() {
  if (m_database.get() == nullptr) {
    initializeDatabase();
  }
  return m_database.get();
}

CryptoNote::Checkpoints *Xi::App::Application::checkpoints() {
  if (m_checkpoints.get() == nullptr) {
    initializeCheckpoints();
  }
  return m_checkpoints.get();
}

CryptoNote::Currency *Xi::App::Application::currency() {
  if (m_currency.get() == nullptr) {
    initializeCurrency();
  }
  return m_currency.get();
}

CryptoNote::ICore *Xi::App::Application::core() {
  if (m_core.get() == nullptr) {
    initializeCore();
  }
  return m_core.get();
}

CryptoNote::INode *Xi::App::Application::remoteNode(bool pollUpdates) {
  if (!m_remoteRpcOptions) {
    return nullptr;
  }
  if (!m_remoteNode) {
    m_remoteNode = std::make_unique<CryptoNote::NodeRpcProxy>(m_remoteRpcOptions->Address, m_remoteRpcOptions->Port,
                                                              m_sslConfig, *m_currency, logger());
    if (!pollUpdates) {
      static_cast<CryptoNote::NodeRpcProxy *>(m_remoteNode.get())->setPollUpdatesEnabled(false);
    }
    m_remoteNode->init().get().throwOnError();
  }
  return m_remoteNode.get();
}

#define XI_APP_CONDITIONAL_OPTION_INIT(MEMBER) \
  if (MEMBER) {                                \
    MEMBER->emplaceOptions(options);           \
  }

void Xi::App::Application::makeOptions(cxxopts::Options &options) {
  CommonCLI::emplaceCLIOptions(options);
  XI_APP_CONDITIONAL_OPTION_INIT(m_logOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_dbOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_remoteRpcOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_netOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_checkpointOptions)
  if (isSSLClientRequired() || isSSLServerRequired()) {
    if (!isSSLClientRequired()) {
      m_sslConfig.emplaceOptions(options, Http::SSLConfiguration::Usage::Server);
    } else if (!isSSLServerRequired()) {
      m_sslConfig.emplaceOptions(options, Http::SSLConfiguration::Usage::Client);
    } else {
      m_sslConfig.emplaceOptions(options, Http::SSLConfiguration::Usage::Both);
    }
  }
}

#undef XI_APP_CONDITIONAL_OPTION_INIT

#define XI_APP_CONDITIONAL_OPTION_EVAL(MEMBER)                       \
  if (MEMBER) {                                                      \
    if (MEMBER->evaluateParsedOptions(options, result)) return true; \
  }

bool Xi::App::Application::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  if (CommonCLI::handleCLIOptions(options, result)) return true;
  XI_APP_CONDITIONAL_OPTION_EVAL(m_logOptions)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_dbOptions)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_remoteRpcOptions)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_netOptions)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_checkpointOptions)
  if (isSSLClientRequired()) {
    if (m_sslConfig.isInsecure(Http::SSLConfiguration::Usage::Client)) {
      (*m_ologger)(Logging::WARNING) << CommonCLI::insecureClientWarning();
    }
  }
  if (isSSLServerRequired()) {
    if (m_sslConfig.isInsecure(Http::SSLConfiguration::Usage::Server)) {
      (*m_ologger)(Logging::WARNING) << CommonCLI::insecureClientWarning();
    }
  }
  return false;
}

void Xi::App::Application::setUp() { initializeLogger(); }

void Xi::App::Application::tearDown() {
  if (m_remoteNode) m_remoteNode->shutdown();
  if (m_core) {
    if (!m_core->save() && m_ologger) {
      (*m_ologger)(Logging::FATAL) << "Core routine save procedure failed.";
    }
  }
  if (m_database) m_database->shutdown();
}

#undef XI_APP_CONDITIONAL_OPTION_EVAL

void Xi::App::Application::useLogging(Logging::Level defaultLevel) {
  if (m_logOptions.get() == nullptr) {
    m_logOptions = std::make_unique<LoggingOptions>();
    m_logOptions->LogFilePath = name() + ".log";
    m_logOptions->DefaultLogLevel = defaultLevel;
  }
}

void Xi::App::Application::useDatabase() {
  if (m_dbOptions.get() == nullptr) {
    m_dbOptions = std::make_unique<DatabaseOptions>();
    useNetwork();
    useLogging();
  }
}

void Xi::App::Application::useRemoteRpc() {
  if (m_remoteRpcOptions.get() == nullptr) {
    m_remoteRpcOptions = std::make_unique<RemoteRpcOptions>();
    useCurrency();
  }
}

void Xi::App::Application::useNetwork() {
  if (m_netOptions.get() == nullptr) {
    m_netOptions = std::make_unique<NetworkOptions>();
  }
}

void Xi::App::Application::useCurrency() {
  useNetwork();
  m_currencyRequired = true;
}

void Xi::App::Application::useCheckpoints() {
  if (m_checkpointOptions == nullptr) {
    m_checkpointOptions = std::make_unique<CheckpointsOptions>();
  }
}

void Xi::App::Application::useCore() {
  useDatabase();
  useNetwork();
  useCheckpoints();
  m_coreRequied = true;
}

bool Xi::App::Application::isSSLClientRequired() const { return m_remoteRpcOptions.get() != nullptr; }

bool Xi::App::Application::isSSLServerRequired() const { return false; }

void Xi::App::Application::initializeLogger() {
  if (m_logOptions) {
    m_consoleLogger =
        std::make_unique<Logging::ConsoleLogger>(m_logOptions->ConsoleLogLevel.value_or(m_logOptions->DefaultLogLevel));
    m_logger->addLogger(*m_consoleLogger);
    auto fileLogLevel = m_logOptions->ConsoleLogLevel.value_or(m_logOptions->DefaultLogLevel);
    if (fileLogLevel != Logging::NONE) {
      auto fileLogger = std::make_unique<Logging::FileLogger>(fileLogLevel);
      fileLogger->init(m_logOptions->LogFilePath);
      m_logger->addLogger(*fileLogger);
      m_fileLogger = std::move(fileLogger);
    }
  }
}

void Xi::App::Application::initializeDatabase() {
  m_database = std::make_unique<CryptoNote::RocksDBWrapper>(logger());
  m_database->init(m_dbOptions->getConfig(m_netOptions->Network));
  if (!CryptoNote::DatabaseBlockchainCache::checkDBSchemeVersion(*m_database, logger())) {
    m_database->shutdown();
    exceptional<ModuleSetUpError>("unable to initialize database.");
  }
}

void Xi::App::Application::initializeCheckpoints() { m_checkpoints = m_checkpointOptions->getCheckpoints(logger()); }

void Xi::App::Application::initializeCurrency() {
  CryptoNote::CurrencyBuilder builder{logger()};
  builder.network(m_netOptions->Network);
  m_currency = std::make_unique<CryptoNote::Currency>(builder.currency());
}

void Xi::App::Application::initializeCore() {
  m_core = std::make_unique<CryptoNote::Core>(
      *currency(), logger(), *checkpoints(), dispatcher(),
      std::make_unique<CryptoNote::DatabaseBlockchainCacheFactory>(*database(), logger()),
      CryptoNote::createSwappedMainChainStorage(m_dbOptions->DataDirectory, *currency()));
  if (!m_core->load()) {
    if (m_ologger) {
      (*m_ologger)(Logging::FATAL) << "Core loading procedure failed.";
      throw std::runtime_error("unable to load core");
    }
  }
}
