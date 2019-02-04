# ============================================================================================== #
#                                                                                                #
#                                       Xi Blockchain                                            #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Galaxia Project - Xi Blockchain                                       #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018-2019 Galaxia Project Developers                                                 #
#                                                                                                #
# This program is free software: you can redistribute it and/or modify it under the terms of the #
# GNU General Public License as published by the Free Software Foundation, either version 3 of   #
# the License, or (at your option) any later version.                                            #
#                                                                                                #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      #
# See the GNU General Public License for more details.                                           #
#                                                                                                #
# You should have received a copy of the GNU General Public License along with this program.     #
# If not, see <https://www.gnu.org/licenses/>.                                                   #
#                                                                                                #
# ============================================================================================== #

add_definitions(-DSTATICLIB)

set(
    XI_INCLUDE_DIRS

    ${PROJECT_SOURCE_DIR}/src
)

file(GLOB_RECURSE BlockchainExplorer BlockchainExplorer/*)
file(GLOB_RECURSE Common Common/*)
file(GLOB_RECURSE Crypto crypto/*)
file(GLOB_RECURSE CryptoNoteCore CryptoNoteCore/* CryptoNoteConfig.h)
file(GLOB_RECURSE CryptoNoteProtocol CryptoNoteProtocol/*)
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
  list(APPEND SYSTEM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows")
elseif(APPLE)
  file(GLOB_RECURSE System System/* Platform/OSX/System/* Platform/Posix/System/*)
  list(APPEND SYSTEM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Platform/OSX" "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Posix")
else()
  file(GLOB_RECURSE System System/* Platform/Linux/System/* Platform/Posix/System/*)
  list(APPEND SYSTEM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Linux" "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Posix")
endif()

foreach(source_file ${Common} ${Crypto} ${CryptoNoteCore} ${CryptoNoteProtocol} ${XI_CLI_DAEMON_SOURCES} ${service}
                   ${JsonRpcServer} ${Logging} ${XI_CLI_MINER_SOURCES} ${Mnemonics} ${NodeRpcProxy} ${P2p}
                   ${Rpc} ${Serialization} ${System} ${Transfers} ${Wallet} ${WalletLegacy} ${XI_CLI_WALLET_SOURCES}
                   ${BlockchainExplorer})
  if(XI_CXX_FLAGS)
    set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "${XI_CXX_FLAGS}")
  endif() # XI_CXX_FLAGS
  source_group("" FILES ${source_file})
endforeach() # source_file

add_library(BlockchainExplorer STATIC ${BlockchainExplorer})
add_library(Common STATIC ${Common})
add_library(Crypto STATIC ${Crypto})
add_library(CryptoNoteCore STATIC ${CryptoNoteCore})
add_library(Logging STATIC ${Logging})
add_library(Mnemonics STATIC ${Mnemonics})
add_library(NodeRpcProxy STATIC ${NodeRpcProxy})
add_library(Rpc STATIC ${Rpc})
add_library(P2P STATIC ${CryptoNoteProtocol} ${P2p})
add_library(Serialization STATIC ${Serialization})
add_library(System STATIC ${System})
target_include_directories(System PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SYSTEM_INCLUDE_DIRS})
add_library(Transfers STATIC ${Transfers})
add_library(Wallet STATIC ${Wallet} ${WalletLegacy})
add_library(JsonRpcServer STATIC ${JsonRpcServer})
add_library(WalletService STATIC ${service})

include(Xi-Version/Version.cmake)

xi_make_library(Core Xi-Core)
target_link_libraries(
  Xi.Core

  PUBLIC
    Xi.Version

    boost
    leathers
    cxxopts
    System
    sparsehash
)

xi_make_library(Crypto Xi-Crypto)
target_link_libraries(
  Xi.Crypto

  PUBLIC
    Xi.Core

  PRIVATE
    openssl
    boost
)

xi_make_library(Config Xi-Config)
target_link_libraries(Xi.Config PUBLIC Xi.Core)

xi_make_library(Http Xi-Http)
target_link_libraries(
  Xi.Http

  PRIVATE
    openssl

  PUBLIC
    Xi.Core
    boost
    cxxopts
    Serialization
    Common
)

if(XI_BUILD_BREAKPAD)
  xi_make_library(CrashHandler Xi-CrashHandler)
  target_link_libraries(Xi.CrashHandler PRIVATE Xi.Version Xi.Config Xi.Http Common gbreakpad boost)
  target_compile_definitions(Xi.CrashHandler INTERFACE XI_USE_BREAKPAD)
endif()

file(GLOB_RECURSE CommonCLI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CommonCLI/*")
add_library(CommonCLI STATIC ${CommonCLI_SOURCES})
target_include_directories(CommonCLI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/CommonCLI")
if(XI_BUILD_BREAKPAD)
  target_link_libraries(CommonCLI PUBLIC cxxopts Xi.Version Xi.Core Common PRIVATE Xi.CrashHandler)
else()
  target_link_libraries(CommonCLI PUBLIC cxxopts Xi.Version Xi.Core Common)
endif()

set(
    XI_LIBRARIES

    BlockchainExplorer
    Common
    CommonCLI
    Crypto
    CryptoNoteCore
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
        Serialization
        System
        rocksdb
        lz4
        boost
)
target_link_libraries(P2P PUBLIC CryptoNoteCore libminiupnpc-static Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(Rpc PUBLIC CryptoNoteCore Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(Wallet NodeRpcProxy Transfers Rpc P2P libminiupnpc-static Serialization CryptoNoteCore System Logging Common Crypto Xi.Core boost json linenoise Xi.Http Xi.Config Xi.Crypto)
target_link_libraries(WalletService PUBLIC Mnemonics CommonCLI boost json linenoise cxxopts Xi.Http Xi.Config Xi.Crypto)
target_link_libraries(BlockchainExplorer PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(Common PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(CommonCLI PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(Crypto PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config Xi.Crypto)
target_link_libraries(Logging PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(Mnemonics PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(NodeRpcProxy PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(P2P PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config Xi.Protocol)
target_link_libraries(Serialization PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(Transfers PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config)
target_link_libraries(JsonRpcServer PUBLIC Xi.Core boost json linenoise Xi.Http Xi.Config Xi.Crypto)

if(XI_BUILD_EXECUTABLES)
  if(MSVC)
    set(XI_CLI_DAEMON_SOURCES_OS
      ${PROJECT_SOURCE_DIR}/src/BinaryInfo/daemon.ico
      ${PROJECT_SOURCE_DIR}/src/BinaryInfo/daemon.rc
    )
    set(XI_CLI_WALLET_SOURCES_OS
      ${PROJECT_SOURCE_DIR}/src/BinaryInfo/wallet.ico
      ${PROJECT_SOURCE_DIR}/src/BinaryInfo/wallet.rc
    )
    set(XI_CLI_MINER_SOURCES_OS
      ${PROJECT_SOURCE_DIR}/src/BinaryInfo/miner.ico
      ${PROJECT_SOURCE_DIR}/src/BinaryInfo/miner.rc
    )
    set(XI_PG_SOURCES_OS
      ${PROJECT_SOURCE_DIR}/src/BinaryInfo/pgservice.ico
      ${PROJECT_SOURCE_DIR}/src/BinaryInfo/paymentgate.rc
    )
  endif()

  add_executable(CLIDaemon ${XI_CLI_DAEMON_SOURCES} ${XI_CLI_DAEMON_SOURCES_OS})
  add_executable(CLIMiner ${XI_CLI_MINER_SOURCES} ${XI_CLI_MINER_SOURCES_OS})
  add_executable(CLIWallet ${XI_CLI_WALLET_SOURCES} ${XI_CLI_WALLET_SOURCES_OS})
  add_executable(PGService ${service} ${XI_PG_SOURCES_OS})

  if(MSVC)
    set(XI_EXTRA_EXEC_LIBRARIES Rpcrt4)

    target_include_directories(CLIDaemon PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/BinaryInfo")
    target_include_directories(CLIMiner PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/BinaryInfo")
    target_include_directories(CLIWallet PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/BinaryInfo")
    target_include_directories(PGService PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/BinaryInfo")
  endif() # MSVC

  target_link_libraries(CLIDaemon P2P Rpc Serialization System Xi.Http Logging CryptoNoteCore Crypto Common CommonCLI rocksdb boost cxxopts ${XI_EXTRA_EXEC_LIBRARIES})
  target_link_libraries(CLIWallet CommonCLI Mnemonics Wallet NodeRpcProxy Transfers Rpc Xi.Http CryptoNoteCore System Logging Common CommonCLI boost cxxopts ${XI_EXTRA_EXEC_LIBRARIES})
  target_link_libraries(CLIMiner CommonCLI CryptoNoteCore Rpc Serialization System Xi.Http Logging Common CommonCLI Crypto boost cxxopts ${XI_EXTRA_EXEC_LIBRARIES})
  target_link_libraries(PGService WalletService JsonRpcServer Wallet NodeRpcProxy Transfers CryptoNoteCore Crypto Rpc Xi.Http System Logging Common rocksdb boost cxxopts ${XI_EXTRA_EXEC_LIBRARIES})

  # Finally build the binaries
  set_property(TARGET CLIDaemon PROPERTY OUTPUT_NAME "xi-daemon")
  set_property(TARGET CLIWallet PROPERTY OUTPUT_NAME "xi-wallet")
  set_property(TARGET CLIMiner PROPERTY OUTPUT_NAME "xi-miner")
  set_property(TARGET PGService PROPERTY OUTPUT_NAME "xi-pgservice")

  # Install targets
  install(
    TARGETS
      CLIDaemon
      CLIWallet
      CLIMiner
      PGService

    PERMISSIONS
      OWNER_EXECUTE OWNER_WRITE OWNER_READ
      GROUP_EXECUTE GROUP_WRITE GROUP_READ
      WORLD_EXECUTE

    RUNTIME DESTINATION
      bin
  )

  # Install license
  install(
    FILES
      "${PROJECT_SOURCE_DIR}/LICENSE"
      "${PROJECT_SOURCE_DIR}/THIRD_PARTY"
      "${PROJECT_SOURCE_DIR}/THIRD_PARTY_LICENSE"

    DESTINATION
      licenses
  )
endif() # XI_BUILD_EXECUTABLES
