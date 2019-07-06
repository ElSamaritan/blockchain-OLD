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

if(DEFINED XI_CMAKE_EXTERNAL_YAML_CPP)
  return()
endif()

set(XI_CMAKE_EXTERNAL_YAML_CPP ON)

set(YAML_CPP_BUILD_TESTS OFF CACHE INTERNAL "Enable testing")
set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "Enable parse tools")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE INTERNAL "Enable contrib stuff in library")
set(YAML_CPP_INSTALL OFF CACHE INTERNAL "Enable generation of install target")
set(MSVC_SHARED_RT OFF CACHE INTERNAL "MSVC: Build with shared runtime libs (/MD)")
set(MSVC_STHREADED_RT OFF CACHE INTERNAL "MSVC: Build with single-threaded static runtime libs (/ML until VS .NET 2003)")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/yaml-cpp)
add_library(yaml::yaml-cpp ALIAS yaml-cpp)
