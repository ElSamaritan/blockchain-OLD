add_definitions(-DSTATICLIB)

set(
    XI_INCLUDE_DIRS

    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/include
    ${XI_VERSION_INCLUDE_DIR}
)

xi_make_library(Core Xi-Core)
target_link_libraries(Xi.Core PUBLIC boost leathers cxxopts) # cxxopts is only linked to enable it for all libraries

xi_make_library(Compression Xi-Compression)
target_link_libraries(
  Xi.Compression

  PUBLIC
    Xi.Core

  PRIVATE
    zlibstatic
    boost
)

xi_make_library(Http Xi-Http)
target_link_libraries(
  Xi.Http

  PUBLIC
    Xi.Core
    boost
)

file(GLOB_RECURSE BlockchainExplorer BlockchainExplorer/*)
file(GLOB_RECURSE Common Common/*)
file(GLOB_RECURSE Crypto crypto/*)
file(GLOB_RECURSE CryptoNoteCore CryptoNoteCore/* CryptoNoteConfig.h)
file(GLOB_RECURSE CryptoNoteProtocol CryptoNoteProtocol/*)
file(GLOB_RECURSE Http HTTP/*)
file(GLOB_RECURSE Logging Logging/*)
file(GLOB_RECURSE Mnemonics Mnemonics/*)
file(GLOB_RECURSE NodeRpcProxy NodeRpcProxy/*)
file(GLOB_RECURSE P2p P2p/*)
file(GLOB_RECURSE Rpc Rpc/*)
file(GLOB_RECURSE Serialization Serialization/*)
file(GLOB_RECURSE Transfers Transfers/*)
file(GLOB_RECURSE Wallet Wallet/*)
file(GLOB_RECURSE WalletLegacy WalletLegacy/*)
file(GLOB_RECURSE JsonRpcServer JsonRpcServer/*)
file(GLOB_RECURSE service WalletService/*)
file(GLOB_RECURSE XI_CLI_DAEMON_SOURCES Daemon/*)
file(GLOB_RECURSE XI_CLI_MINER_SOURCES Miner/*)
file(GLOB_RECURSE XI_CLI_WALLET_SOURCES zedwallet/*)

if(MSVC)
  file(GLOB_RECURSE System System/* Platform/Windows/System/*)
  list(APPEND XI_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows")
elseif(APPLE)
  file(GLOB_RECURSE System System/* Platform/OSX/System/* Platform/Posix/System/*)
  list(APPEND XI_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Platform/OSX" "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Posix")
else()
  file(GLOB_RECURSE System System/* Platform/Linux/System/* Platform/Posix/System/*)
  list(APPEND XI_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Platform/OSX" "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Posix")
endif()

# This appears to be an IDE thing, to group files together.
# https://cmake.org/cmake/help/v3.0/command/source_group.html
# Probably not what you need to be looking at if something isn't building
foreach(source_file ${Common} ${Crypto} ${CryptoNoteCore} ${CryptoNoteProtocol} ${XI_CLI_DAEMON_SOURCES} ${service}
                   ${JsonRpcServer} ${Http} ${Logging} ${XI_CLI_MINER_SOURCES} ${Mnemonics} ${NodeRpcProxy} ${P2p}
                   ${Rpc} ${Serialization} ${System} ${Transfers} ${Wallet} ${WalletLegacy} ${XI_CLI_WALLET_SOURCES}
                   ${BlockchainExplorer})
  if(XI_CXX_FLAGS)
    set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "${XI_CXX_FLAGS}")
  endif() # XI_CXX_FLAGS
  source_group("" FILES ${source_file})
endforeach()

add_library(BlockchainExplorer STATIC ${BlockchainExplorer})
add_library(Common STATIC ${Common})
add_library(Crypto STATIC ${Crypto})
add_library(CryptoNoteCore STATIC ${CryptoNoteCore})
add_library(Http STATIC ${Http})
add_library(Logging STATIC ${Logging})
add_library(Mnemonics STATIC ${Mnemonics})
add_library(NodeRpcProxy STATIC ${NodeRpcProxy})
add_library(Rpc STATIC ${Rpc})
add_library(P2P STATIC ${CryptoNoteProtocol} ${P2p})
add_library(Serialization STATIC ${Serialization})
add_library(System STATIC ${System})
add_library(Transfers STATIC ${Transfers})
add_library(Wallet STATIC ${Wallet} ${WalletLegacy})
add_library(JsonRpcServer STATIC ${JsonRpcServer})
add_library(WalletService STATIC ${service})

file(GLOB_RECURSE CommonCLI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CommonCLI/*")
add_library(CommonCLI STATIC ${CommonCLI_SOURCES})
target_include_directories(CommonCLI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/CommonCLI")
add_dependencies(CommonCLI Version)

set(
    XI_LIBRARIES

    BlockchainExplorer
    Common
    CommonCLI
    Crypto
    CryptoNoteCore
    Http
    Logging
    Mnemonics
    NodeRpcProxy
    Rpc
    P2P
    Serialization
    System
    Transfers
    Wallet
    JsonRpcServer
    WalletService
)

foreach(xiLib ${XI_LIBRARIES})
  target_include_directories(${xiLib} PUBLIC ${XI_INCLUDE_DIRS})
endforeach()

if(MSVC)
  target_link_libraries(System ws2_32 Xi.Core)
else()
  target_link_libraries(System Xi.Core)
endif ()

# Add the dependencies we need
target_link_libraries(
    CryptoNoteCore

    PUBLIC
        Common
        Logging
        Crypto
        P2P
        Rpc
        Http
        Serialization
        System
        rocksdb
        lz4
        boost
)
target_link_libraries(P2P PUBLIC CryptoNoteCore libminiupnpc-static Xi.Core boost json cpp-linenoise)
target_link_libraries(Rpc PUBLIC CryptoNoteCore Xi.Core boost json cpp-linenoise)
target_link_libraries(Wallet NodeRpcProxy Transfers Rpc P2P libminiupnpc-static Http Serialization CryptoNoteCore System Logging Common Crypto Xi.Core boost json cpp-linenoise)
target_link_libraries(WalletService PUBLIC Mnemonics CommonCLI boost json cpp-linenoise cxxopts)
target_link_libraries(BlockchainExplorer PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(Common PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(CommonCLI PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(Crypto PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(Http PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(Logging PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(Mnemonics PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(NodeRpcProxy PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(P2P PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(Serialization PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(Transfers PUBLIC Xi.Core boost json cpp-linenoise)
target_link_libraries(JsonRpcServer PUBLIC Xi.Core boost json cpp-linenoise)

add_dependencies(WalletService Version)
add_dependencies(Rpc Version)
add_dependencies(P2P Version)

if(XI_BUILD_EXECUTABLES)
  if(MSVC)
    set(XI_CLI_DAEMON_SOURCES_OS
      BinaryInfo/daemon.rc
    )
    set(XI_CLI_WALLET_SOURCES_OS
      BinaryInfo/wallet.rc
    )
    set(XI_CLI_MINER_SOURCES_OS
      BinaryInfo/miner.rc
    )
    set(XI_PG_SOURCES_OS
      BinaryInfo/paymentgate.rc
    )
  endif()

  add_executable(CLIDaemon ${XI_CLI_DAEMON_SOURCES} ${XI_CLI_DAEMON_SOURCES_OS})
  add_executable(CLIMiner ${XI_CLI_MINER_SOURCES} ${XI_CLI_MINER_SOURCES_OS})
  add_executable(CLIWallet ${XI_CLI_WALLET_SOURCES} ${XI_CLI_WALLET_SOURCES_OS})
  add_executable(PGService ${service} ${XI_PG_SOURCES_OS})

  if(MSVC)
    set(XI_EXTRA_OS_LIBRARIES Rpcrt4)
  endif()

  target_link_libraries(CLIDaemon P2P Rpc Serialization System Http Logging CryptoNoteCore Crypto Common CommonCLI rocksdb boost cxxopts ${XI_EXTRA_OS_LIBRARIES})
  target_link_libraries(CLIWallet CommonCLI Mnemonics Wallet NodeRpcProxy Transfers Rpc Http CryptoNoteCore System Logging Common CommonCLI boost cxxopts)
  target_link_libraries(CLIMiner CommonCLI CryptoNoteCore Rpc Serialization System Http Logging Common CommonCLI Crypto boost cxxopts)
  target_link_libraries(PGService WalletService JsonRpcServer Wallet NodeRpcProxy Transfers CryptoNoteCore Crypto Rpc Http System Logging Common rocksdb boost cxxopts ${XI_EXTRA_OS_LIBRARIES})

  # Add dependencies means we have to build the latter before we build the former
  # In this case it's because we need to have the current version name rather
  # than a cached one
  add_dependencies(CLIDaemon Version)
  add_dependencies(CLIWallet Version)
  add_dependencies(CLIMiner Version)
  add_dependencies(PGService Version)

  # Finally build the binaries
  set_property(TARGET CLIDaemon PROPERTY OUTPUT_NAME "xi-daemon")
  set_property(TARGET CLIWallet PROPERTY OUTPUT_NAME "xi-wallet")
  set_property(TARGET CLIMiner PROPERTY OUTPUT_NAME "xi-miner")
  set_property(TARGET PGService PROPERTY OUTPUT_NAME "xi-pgservice")

  # Install targets
  install(
    TARGETS CLIDaemon CLIWallet CLIMiner PGService
    RUNTIME DESTINATION bin
  )
endif() # XI_BUILD_EXECUTABLES
