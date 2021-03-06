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

cmake_policy(PUSH)
if(CMAKE_VERSION VERSION_GREATER "3.12")
  cmake_policy(SET CMP0074 OLD)
endif()

set(OPENSSL_USE_STATIC_LIBS ON)
if(MSVC)
  set(OPENSSL_MSVC_STATIC_RT ON)
endif()

find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
cmake_policy(POP)

if(UNIX)
  set(OPENSSL_EXTRA_LIBS dl)
else()
  set(OPENSSL_EXTRA_LIBS)
endif()

add_library(openssl INTERFACE IMPORTED GLOBAL)
target_include_directories(openssl INTERFACE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(openssl INTERFACE ${OPENSSL_LIBRARIES} ${OPENSSL_EXTRA_LIBS})

set(OPENSSL_VERSION ${OPENSSL_VERSION} CACHE STRING "OpenSSL Version" FORCE)
mark_as_advanced(
  OPENSSL_VERSION
  SSL_EAY_DEBUG
  SSL_EAY_RELEASE
  LIB_EAY_DEBUG
  LIB_EAY_RELEASE
)

