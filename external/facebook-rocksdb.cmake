﻿# ============================================================================================== #
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

if(MSVC)
  set(
    ROCKSB_EXTRA_CMAKE_ARGS
      -DROCKSDB_INSTALL_ON_WINDOWS=ON
      -DCMAKE_C_FLAGS="/MP"
      -DCMAKE_CXX_FLAGS="/MP"
      -DCMAKE_SHARED_LINKER_FLAGS="/ignore:4221"
      -DCMAKE_STATIC_LINKER_FLAGS="/ignore:4221"
  )
elseif(XI_C_COMPILER_AppleClang OR XI_CXX_COMPILER_AppleClang)
  set(
    ROCKSDB_EXTRA_CMAKE_ARGS
      -DENABLE_PTHREAD_MUTEX_ADAPTIVE_NP=OFF
      -DCMAKE_SHARED_LINKER_FLAGS="-no_warning_for_no_symbols"
      -DCMAKE_STATIC_LINKER_FLAGS="-no_warning_for_no_symbols"
    )
else()
  set(ROCKSDB_EXTRA_CMAKE_ARGS -DENABLE_PTHREAD_MUTEX_ADAPTIVE_NP=OFF)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(ROCKSDB_BUILD_TYPE "Debug")
else()
  set(ROCKSDB_BUILD_TYPE "Release")
endif()

ExternalProject_Add(
  facebook-rocksdb

  PREFIX "facebook-rocksdb"

  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  TEST_COMMAND ""

  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/facebook-rocksdb"
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/rocksdb

  CMAKE_ARGS
    -DWITH_MD_LIBRARY=OFF
    -DWITH_TESTS=OFF
    -DWITH_TOOLS=OFF
    -DPORTABLE=ON
    -DWITH_RUNTIME_DEBUG=ON
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/rocksdb
    -DCMAKE_BUILD_TYPE=${ROCKSDB_BUILD_TYPE}
    -DUSE_RTTI=ON
    -DLZ4_ROOT_DIR=${LZ4_ROOT_DIR}
    -DWITH_LZ4=ON
    ${ROCKSB_EXTRA_CMAKE_ARGS}

  INSTALL_COMMAND ${CMAKE_COMMAND} -DCOMPONENT=devel -P cmake_install.cmake
)
add_dependencies(facebook-rocksdb lz4-lz4)

install()
ExternalProject_Get_Property(facebook-rocksdb INSTALL_DIR)

add_library(rocksdb STATIC IMPORTED GLOBAL)
add_dependencies(rocksdb facebook-rocksdb)

include(GNUInstallDirs)
if(MSVC)
  set_property(TARGET rocksdb PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/rocksdb${CMAKE_STATIC_LIBRARY_SUFFIX})
  set_property(TARGET rocksdb PROPERTY INTERFACE_LINK_LIBRARIES Shlwapi.lib Rpcrt4.lib)
else()
  set_property(TARGET rocksdb PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/librocksdb${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
set_property(TARGET rocksdb APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}/include")
