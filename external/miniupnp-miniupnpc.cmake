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

set(UPNPC_BUILD_STATIC ON CACHE BOOL "Build static library" FORCE)
set(UPNPC_BUILD_SHARED OFF CACHE BOOL "Build shared library" FORCE)
set(UPNPC_BUILD_TESTS OFF CACHE BOOL "Build test executables" FORCE)
set(UPNPC_BUILD_SAMPLE OFF CACHE BOOL "Build sample executables" FORCE)

add_subdirectory(miniupnp/miniupnpc EXCLUDE_FROM_ALL)
target_include_directories(libminiupnpc-static INTERFACE ${PROJECT_SOURCE_DIR}/external/miniupnp)

mark_as_advanced(
  UPNPC_BUILD_STATIC
  UPNPC_BUILD_SHARED
  UPNPC_BUILD_TESTS
  UPNPC_BUILD_SAMPLE
  NO_GETADDRINFO
)
