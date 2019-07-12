# ============================================================================================== #
#                                                                                                #
#                                     Galaxia Blockchain                                         #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Xi framework.                                                         #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               #
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

foreach(source_file ${Common} ${Crypto} ${CryptoNoteCore} ${CryptoNoteProtocol}
                   ${JsonRpcServer} ${Logging} ${Mnemonics} ${NodeRpcProxy} ${P2p}
                   ${Rpc} ${Serialization} ${System} ${Transfers} ${Wallet} ${WalletLegacy}
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

include(Xi-Version/Version.cmake)

xi_make_library(
  Xi-Core

  PUBLIC_LIBRARIES
    Xi.Version

    boost
    leathers
    cxxopts
    System
    sparsehash
    fmt::fmt-header-only
)

xi_make_library(Xi-Endianess)

target_compile_definitions(
    Xi.Endianess

    INTERFACE
        "XI_COMPILER_ENDIANESS_${XI_COMPILER_ENDIANESS}"
)

xi_make_library(
  Xi-Crypto

  PUBLIC_LIBRARIES
    Xi::Core
    Xi::Endianess

    Serialization

  PRIVATE_LIBRARIES
    openssl
    boost
)


xi_make_library(
  Xi-Http

  PRIVATE_LIBRARIES
    openssl

  PUBLIC_LIBRARIES
    Xi.Core
    boost
    cxxopts
    Serialization
    Common
)

xi_make_library(
  Xi-Rpc

  PUBLIC_LIBRARIES
    Xi::Core
    Xi::Http
    Serialization
    Logging
)

xi_make_library(
  Xi-Blockchain

  PUBLIC_LIBRARIES
    Xi::Core
    Xi::Crypto
    Xi::Rpc
    Crypto
    Serialization
    Logging
    CryptoNoteCore
)

xi_make_library(
  Xi-Config

  PUBLIC_LIBRARIES
    Xi::Core
    Xi::Blockchain
)

if(XI_BUILD_BREAKPAD)
  xi_make_library(
    Xi-CrashHandler

    PRIVATE_LIBRARIES
      Xi.Version
      Xi.Config
      Xi.Http
      Common
      gbreakpad
      boost
  )
  target_compile_definitions(Xi.CrashHandler INTERFACE XI_USE_BREAKPAD)
endif()

xi_make_library(
  Xi-App

  PUBLIC_LIBRARIES
    Xi.Core
    boost
    cxxopts
    Serialization
    Common
    CommonCLI
    CryptoNoteCore
    NodeRpcProxy
)

file(GLOB_RECURSE CommonCLI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/CommonCLI/*")
add_library(CommonCLI STATIC ${CommonCLI_SOURCES})
target_include_directories(CommonCLI PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/CommonCLI")
if(XI_BUILD_BREAKPAD)
  target_link_libraries(CommonCLI PUBLIC cxxopts Xi::Core Xi::Version Common PRIVATE Xi::CrashHandler)
else()
  target_link_libraries(CommonCLI PUBLIC cxxopts Xi::Core Xi::Version Common)
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
)
set(XI_LIBRARIES ${XI_LIBRARIES} PARENT_SCOPE)

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

    PRIVATE
        Async++
)

# The reason we link so many Xi libraries here:
# TRTLs code base included interface definitions accross it's boundaries without excplicit dependency statements. Xi
# libraries have a more restrictive include policy. As a result any legacy library using any Xi library may lead to
# a compiler error in case it does not explicitly state it's dependency to the Xi library used in the interface not
# linked explicitly.
target_link_libraries(P2P PUBLIC CryptoNoteCore libminiupnpc-static boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(Rpc PUBLIC CryptoNoteCore boost json linenoise BlockchainExplorer ${XI_CANONICAL_LIBRARIES})
target_link_libraries(Wallet NodeRpcProxy Transfers Rpc P2P libminiupnpc-static Serialization CryptoNoteCore System Logging Common Crypto boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(BlockchainExplorer PUBLIC boost json linenoise Crypto ${XI_CANONICAL_LIBRARIES})
target_link_libraries(Common PUBLIC boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(CommonCLI PUBLIC boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(Crypto PUBLIC boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(Logging PUBLIC boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(Mnemonics PUBLIC boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(NodeRpcProxy PUBLIC boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(P2P PUBLIC boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(Serialization PUBLIC boost json linenoise rang::rang yaml::yaml-cpp ${XI_CANONICAL_LIBRARIES})
target_link_libraries(Transfers PUBLIC boost json linenoise ${XI_CANONICAL_LIBRARIES})
target_link_libraries(JsonRpcServer PUBLIC boost json linenoise ${XI_CANONICAL_LIBRARIES})
