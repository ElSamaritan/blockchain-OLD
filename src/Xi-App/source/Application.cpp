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
#include <ctime>
#include <string>

#include <Xi/ExternalIncludePush.h>
#include <boost/algorithm/string.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Exceptional.hpp>
#include <Xi/FileSystem.h>
#include <Xi/Log/Discord/Discord.hpp>
#include <Xi/Version/BuildInfo.h>
#include <Xi/Version/Version.h>
#include <CommonCLI/CommonCLI.h>
#include <Logging/ConsoleLogger.h>
#include <Logging/FileLogger.h>
#include <CryptoNoteCore/Core.h>
#include <CryptoNoteCore/DatabaseBlockchainCache.h>
#include <CryptoNoteCore/DatabaseBlockchainCacheFactory.h>
#include <CryptoNoteCore/MainChainStorage.h>
#include <NodeRpcProxy/NodeRpcProxy.h>
#include <NodeInProcess/NodeInProcess.hpp>

#if defined(XI_USE_BREAKPAD)
#include <Xi/CrashHandler.h>
#endif

#if defined(_WIN32) && defined(NDEBUG)
#include <Windows.h>
#endif

#include "Xi/App/Environment.h"

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

#define XI_APP_REMOVE_LOGGER(MEMBER) \
  if (MEMBER) {                      \
    m_logger->removeLogger(*MEMBER); \
    MEMBER.reset(nullptr);           \
  }

Xi::App::Application::~Application() {
  XI_APP_REMOVE_LOGGER(m_consoleLogger)
  XI_APP_REMOVE_LOGGER(m_fileLogger)
  XI_APP_REMOVE_LOGGER(m_discordLogger)
  m_ologger.reset(nullptr);
  m_logger.reset(nullptr);
}

#undef XI_APP_REMOVE_LOGGER

int Xi::App::Application::exec(int argc, char **argv) {
#if defined(_WIN32) && defined(NDEBUG)
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif

  try {
    Environment env{"XI"};
    Environment::set(std::string{"XI_APP="} + name());
    Environment::set(std::string{"XI_TIMESTAMP="} + std::to_string(std::time(nullptr)));
    Environment::set(std::string{"XI_VERSION="} + APP_VERSION);
    Environment::set(std::string{"XI_VERSION_PATCH="} + std::to_string(APP_VER_REV));
    Environment::set(std::string{"XI_VERSION_MINOR="} + std::to_string(APP_VER_MINOR));
    Environment::set(std::string{"XI_VERSION_MAJOR="} + std::to_string(APP_VER_MAJOR));
    Environment::set(std::string{"XI_BUILD_BRANCH="} + BUILD_BRANCH);
    Environment::set(std::string{"XI_BUILD_CHANNEL="} + BUILD_CHANNEL);
    Environment::set(std::string{"XI_BUILD_COMMIT="} + BUILD_COMMIT_ID);

    env.load();
    env.load(std::string{".env."} + to_lower(replace(name(), "xi-", "")));

    for (int i = 1; i < argc; ++i) {
      std::string iarg = argv[i];
      if (starts_with(iarg, "ENV=")) {
        std::string envvalue = iarg.substr(4);
        env.load(envvalue);
        std::swap(argv[i], argv[argc - 1]);
        argc -= 1;
      }
    }
    loadEnvironment(env);

    cxxopts::Options cliOptions{name(), description()};
    makeOptions(cliOptions);
    const auto parseResult = cliOptions.parse(argc, argv);
    if (evaluateParsedOptions(cliOptions, parseResult)) {
      return EXIT_SUCCESS;
    }

#if defined(XI_USE_BREAKPAD)
    std::unique_ptr<CrashHandler> breakpad{};
    if (m_breakpadOptions.enabled()) {
      CrashHandlerConfig breakpadConfig{};
      breakpadConfig.IsEnabled = true;
      breakpadConfig.IsUploadEnabled = false;
      breakpadConfig.OutputPath = m_breakpadOptions.outputPath();
      breakpadConfig.Application = name();
      breakpad = std::make_unique<CrashHandler>(breakpadConfig);
    }
#endif

    if (CommonCLI::isDevVersion()) {
      (*m_ologger)(Logging::Warning) << CommonCLI::devWarning();
    }

    if (m_sslClientOptions && ssl()->isInsecure(Http::SSLConfiguration::Usage::Client)) {
      (*m_ologger)(Logging::Warning) << CommonCLI::insecureClientWarning();
    }

    if (m_sslServerOptions && ssl()->isInsecure(Http::SSLConfiguration::Usage::Server)) {
      (*m_ologger)(Logging::Warning) << CommonCLI::insecureServerWarning();
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

const std::string Xi::App::Application::name() const {
  return m_name;
}

const std::string Xi::App::Application::description() const {
  return m_description;
}

Logging::LoggerManager &Xi::App::Application::logger() {
  return *m_logger;
}

System::Dispatcher &Xi::App::Application::dispatcher() {
  return m_dispatcher;
}

CryptoNote::RpcRemoteConfiguration Xi::App::Application::remoteConfiguration() {
  return m_remoteRpcOptions->getConfig(*ssl());
}

CryptoNote::RocksDBWrapper *Xi::App::Application::database() {
  if (m_database.get() == nullptr) {
    initializeDatabase();
  }
  return m_database.get();
}

std::string Xi::App::Application::dataDirectory() {
  database();
  return m_dbOptions->DataDirectory;
}

CryptoNote::Checkpoints *Xi::App::Application::checkpoints() {
  if (m_checkpoints.get() == nullptr) {
    initializeCheckpoints();
  }
  return m_checkpoints.get();
}

CryptoNote::CurrencyBuilder *Xi::App::Application::intermediateCurrency() {
  if (m_intermediateCurrency.get() == nullptr) {
    initializeIntermediateCurrency();
  }
  return m_intermediateCurrency.get();
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

CryptoNote::INode *Xi::App::Application::rpcNode(bool pollUpdates, bool preferEmbbedded) {
  if (!m_remoteRpcOptions && !m_nodeOptions) {
    return nullptr;
  }
  if (!m_remoteNode) {
    if (!preferEmbbedded || !m_remoteRpcOptions) {
      auto &crrcy = *currency();
      auto &sslConf = *ssl();
      m_remoteNode = std::make_unique<CryptoNote::NodeRpcProxy>(m_remoteRpcOptions->address(),
                                                                m_remoteRpcOptions->port(), sslConf, crrcy, logger());
      if (!m_remoteRpcOptions->accessToken().empty()) {
        static_cast<CryptoNote::NodeRpcProxy *>(m_remoteNode.get())
            ->httpClient()
            .useAuthorization(Xi::Http::BearerCredentials{m_remoteRpcOptions->accessToken()});
      }

      if (!pollUpdates) {
        static_cast<CryptoNote::NodeRpcProxy *>(m_remoteNode.get())->setPollUpdatesEnabled(false);
      }
    } else {
      auto &internalNode = *node();
      m_remoteNode = std::make_unique<CryptoNote::NodeInProcess>(internalNode, *m_protocol, *core(), logger());
    }
  }
  return m_remoteNode.get();
}

CryptoNote::NodeServer *Xi::App::Application::node() {
  if (!m_nodeOptions) {
    return nullptr;
  }

  if (!m_node) {
    initializeNode();
  }

  return m_node.get();
}

Xi::Http::SSLConfiguration *Xi::App::Application::ssl() {
  if (!m_sslConfig) {
    initializeSsl();
  }
  return m_sslConfig.get();
}

CryptoNote::RpcServer *Xi::App::Application::rpcServer() {
  if (!m_rpcServer) {
    initializeRpcServer();
  }
  return m_rpcServer.get();
}

CryptoNote::CryptoNoteProtocolHandler *Xi::App::Application::protocol() {
  node();
  return m_protocol.get();
}

#define XI_APP_OPTION_ENV(MEMBER) MEMBER.loadEnvironment(env);

#define XI_APP_CONDITIONAL_OPTION_ENV(MEMBER) \
  if (MEMBER) {                               \
    XI_APP_OPTION_ENV((*MEMBER));             \
  }

void Xi::App::Application::loadEnvironment(Xi::App::Environment &env) {
  XI_APP_OPTION_ENV(m_generalOptions)
  XI_APP_OPTION_ENV(m_licenseOptions)
  XI_APP_OPTION_ENV(m_breakpadOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_logOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_dbOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_remoteRpcOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_netOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_checkpointOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_nodeOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_rpcServerOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_blockExplorerOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_publicNodeOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_sslOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_sslServerOptions)
  XI_APP_CONDITIONAL_OPTION_ENV(m_sslClientOptions)
}

#undef XI_APP_CONDITIONAL_ENV
#undef XI_APP_CONDITIONAL_OPTION_ENV

#define XI_APP_OPTION_INIT(MEMBER) MEMBER.emplaceOptions(options);

#define XI_APP_CONDITIONAL_OPTION_INIT(MEMBER) \
  if (MEMBER) {                                \
    XI_APP_OPTION_INIT((*MEMBER));             \
  }

void Xi::App::Application::makeOptions(cxxopts::Options &options) {
  XI_APP_OPTION_INIT(m_generalOptions)
  XI_APP_OPTION_INIT(m_licenseOptions)
  XI_APP_OPTION_INIT(m_breakpadOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_logOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_dbOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_remoteRpcOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_netOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_checkpointOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_nodeOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_rpcServerOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_blockExplorerOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_publicNodeOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_sslOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_sslServerOptions)
  XI_APP_CONDITIONAL_OPTION_INIT(m_sslClientOptions)
}

#undef XI_APP_CONDITIONAL_OPTION_INIT

#define XI_APP_OPTION_EVAL(MEMBER, DESC)                                       \
  try {                                                                        \
    if (MEMBER.evaluateParsedOptions(options, result))                         \
      return true;                                                             \
  } catch (const std::exception &e) {                                          \
    std::cerr << "Invalid " << #DESC << " options: " << e.what() << std::endl; \
    throw e;                                                                   \
  }

#define XI_APP_CONDITIONAL_OPTION_EVAL(MEMBER, DESC) \
  if (MEMBER) {                                      \
    XI_APP_OPTION_EVAL((*MEMBER), DESC)              \
  }

bool Xi::App::Application::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_APP_OPTION_EVAL(m_generalOptions, general)
  XI_APP_OPTION_EVAL(m_licenseOptions, license)
  XI_APP_OPTION_EVAL(m_breakpadOptions, breakpad)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_logOptions, logging)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_dbOptions, database)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_remoteRpcOptions, rpcremote)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_netOptions, network)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_checkpointOptions, checkpoint)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_nodeOptions, node)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_rpcServerOptions, node)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_blockExplorerOptions, node)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_publicNodeOptions, node)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_sslOptions, ssl)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_sslServerOptions, ssl)
  XI_APP_CONDITIONAL_OPTION_EVAL(m_sslClientOptions, ssl)
  XI_RETURN_SC(false);
}

#undef XI_APP_OPTION_EVAL
#undef XI_APP_CONDITIONAL_OPTION_EVAL

void Xi::App::Application::setUp() {
  initializeLogger();
}

void Xi::App::Application::tearDown() {
  if (m_node) {
    if (!m_node->deinit()) {
      (*m_ologger)(Logging::Fatal) << "node server shutdown failed";
    }
  }

  if (m_rpcServer) {
    try {
      m_rpcServer->stop();
    } catch (std::exception &e) {
      (*m_ologger)(Logging::Fatal) << "rpc srver shutdown faield: " << e.what();
    }
  }

  if (m_remoteNode) {
    m_remoteNode->shutdown();
  }
  if (m_core) {
    if (!m_core->transactionPool().save(m_dbOptions->DataDirectory)) {
      (*m_ologger)(Logging::Fatal) << "transaction pool export failed.";
    }

    if (!m_core->save() && m_ologger) {
      (*m_ologger)(Logging::Fatal) << "Core routine save procedure failed.";
    }
  }
  if (m_database)
    m_database->shutdown();
}

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
    useCurrency();
    useLogging();
  }
}

void Xi::App::Application::useRemoteRpc() {
  useCurrency();
  useSslClient();
  if (m_remoteRpcOptions.get() == nullptr) {
    m_remoteRpcOptions = std::make_unique<RemoteRpcOptions>();
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
  useCurrency();
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

void Xi::App::Application::useNode() {
  useCore();
  if (m_nodeOptions == nullptr) {
    m_nodeOptions = std::make_unique<NodeOptions>();
  }
}

void Xi::App::Application::useRpcServer() {
  useSslServer();
  useCurrency();
  useCore();
  useNode();
  if (m_rpcServerOptions == nullptr) {
    m_rpcServerOptions = std::make_unique<ServerRpcOptions>();
    m_blockExplorerOptions = std::make_unique<BlockExplorerOptions>();
    m_publicNodeOptions = std::make_unique<PublicNodeOptions>();
  }
}

void Xi::App::Application::useSsl() {
  if (m_sslOptions == nullptr) {
    m_sslOptions = std::make_unique<SslOptions>();
  }
}

void Xi::App::Application::useSslServer() {
  useSsl();
  if (m_sslServerOptions == nullptr) {
    m_sslServerOptions = std::make_unique<SslServerOptions>(*m_sslOptions);
  }
}

void Xi::App::Application::useSslClient() {
  useSsl();
  if (m_sslClientOptions == nullptr) {
    m_sslClientOptions = std::make_unique<SslClientOptions>(*m_sslOptions);
  }
}

void Xi::App::Application::initializeLogger() {
  if (m_logOptions) {
    m_logger->setMaxLevel(m_logOptions->DefaultLogLevel);
    m_consoleLogger = std::make_unique<Logging::ConsoleLogger>(m_logOptions->ConsoleLogLevel.value_or(Logging::Trace));
    m_logger->addLogger(*m_consoleLogger);
    auto fileLogLevel = m_logOptions->FileLogLevel.value_or(Logging::Trace);
    if (fileLogLevel != Logging::None) {
      auto fileLogger = std::make_unique<Logging::FileLogger>(fileLogLevel);
      auto filePath = m_logOptions->LogFilePath;
      if (filePath.empty()) {
        filePath = std::string{"./"} + name() + ".log";
      }
      fileLogger->init(m_logOptions->LogFilePath);
      m_logger->addLogger(*fileLogger);
      m_fileLogger = std::move(fileLogger);
    }

    if (!m_logOptions->DiscordWebhook.empty()) {
      auto discordLogger = std::make_unique<Xi::Log::Discord::DiscordLogger>(
          m_logOptions->DiscordWebhook, m_logOptions->DiscordLogLevel.value_or(Logging::Trace));
      discordLogger->setAuthor(m_logOptions->DiscordAuthor);
      m_logger->addLogger(*discordLogger);
      m_discordLogger = std::move(discordLogger);
    }
  }
}

void Xi::App::Application::initializeDatabase() {
  m_database = std::make_unique<CryptoNote::RocksDBWrapper>(logger());
  m_dbOptions->DataDirectory = m_dbOptions->DataDirectory + std::string{"/"} + currency()->networkUniqueName();
  FileSystem::ensureDirectoryExists(m_dbOptions->DataDirectory).throwOnError();
  m_database->init(m_dbOptions->getConfig());
  if (!CryptoNote::DatabaseBlockchainCache::checkDBSchemeVersion(*m_database, logger())) {
    m_database->shutdown();
    exceptional<ModuleSetUpError>("unable to initialize database.");
  }
}

void Xi::App::Application::initializeCheckpoints() {
  m_checkpoints = m_checkpointOptions->getCheckpoints(*currency(), logger());
}

void Xi::App::Application::initializeIntermediateCurrency() {
  m_intermediateCurrency = std::make_unique<CryptoNote::CurrencyBuilder>(logger());
  m_intermediateCurrency->networkDir(m_netOptions->directory());
  m_intermediateCurrency->network(m_netOptions->network());
}

void Xi::App::Application::initializeCurrency() {
  CryptoNote::CurrencyBuilder builder{logger()};
  builder.networkDir(m_netOptions->directory());
  builder.network(m_netOptions->network());
  m_currency = std::make_unique<CryptoNote::Currency>(builder.currency());
}

void Xi::App::Application::initializeCore() {
  database();
  m_core = std::make_unique<CryptoNote::Core>(
      *currency(), logger(), *checkpoints(), dispatcher(), m_dbOptions->LightNode,
      std::make_unique<CryptoNote::DatabaseBlockchainCacheFactory>(*database(), logger()),
      CryptoNote::createSwappedMainChainStorage(m_dbOptions->DataDirectory, *currency()));
  if (!m_core->load()) {
    if (m_ologger) {
      (*m_ologger)(Logging::Fatal) << "Core loading procedure failed.";
      throw std::runtime_error("unable to load core");
    }
  }

  if (!m_core->transactionPool().load(m_dbOptions->DataDirectory)) {
    (*m_ologger)(Logging::Fatal) << "Transaction pool loading procedure failed.";
    throw std::runtime_error("unable to load core");
  }
}

void Xi::App::Application::initializeNode() {
  m_protocol =
      std::make_unique<CryptoNote::CryptoNoteProtocolHandler>(*currency(), dispatcher(), *core(), nullptr, logger());
  auto config = m_nodeOptions->getConfig(m_dbOptions->DataDirectory, *currency());
  m_node = std::make_unique<CryptoNote::NodeServer>(dispatcher(), currency()->network(), *m_protocol, logger());
  m_protocol->set_p2p_endpoint(m_node.get());
  exceptional_if_not<RuntimeError>(m_node->init(config), "unable to initialize local node");
}

void Xi::App::Application::initializeSsl() {
  m_sslConfig = std::make_unique<Http::SSLConfiguration>();
  m_sslOptions->configure(*m_sslConfig);

  if (m_sslServerOptions) {
    m_sslServerOptions->configure(*m_sslConfig);
  }

  if (m_sslServerOptions) {
    m_sslServerOptions->configure(*m_sslConfig);
  }
}

void Xi::App::Application::initializeRpcServer() {
  const auto &config = *m_rpcServerOptions;
  if (config.enabled()) {
    m_rpcServer = std::make_shared<CryptoNote::RpcServer>(
        dispatcher(), logger(), static_cast<CryptoNote::Core &>(*core()), *node(), *protocol());
    m_rpcServer->setAccessToken(config.accessToken());
    m_rpcServer->enableCors(config.cors());

    if (!m_publicNodeOptions->fee().address().empty()) {
      m_rpcServer->setFeeAddress(m_publicNodeOptions->fee().address());
      m_rpcServer->setFeeAmount(m_publicNodeOptions->fee().amount());
      if (!m_publicNodeOptions->fee().viewKey().empty()) {
        m_rpcServer->setFeeViewKey(m_publicNodeOptions->fee().viewKey());
      }
    }

    if (m_blockExplorerOptions->enabled()) {
      m_rpcServer->setBlockexplorer(true);
      if (!m_publicNodeOptions->enabled()) {
        m_rpcServer->setBlockexplorerOnly(true);
      }
    }

    m_rpcServer->setHandler(m_rpcServer);
    m_rpcServer->start(config.bind(), config.port());
  }
}
