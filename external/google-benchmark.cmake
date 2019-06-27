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

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "TestSuite of google-benchmark disabled" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "TestSuite of google-benchmark disabled" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Install targets for google-benchmark disabled" FORCE)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undef")
endif()

if(MSVC)
  # MSVC currently fails the cxx_feature_check(STD_REGEX) test when compiled with c++17
  set(HAVE_STD_REGEX ON)
endif()

add_subdirectory(google-benchmark)

if(MSVC)
  unset(HAVE_STD_REGEX)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${_CMAKE_CXX_FLAGS}")
endif()

set_property(TARGET benchmark benchmark_main PROPERTY FOLDER "external")
mark_as_advanced(
    BENCHMARK_ENABLE_TESTING
    BENCHMARK_ENABLE_GTEST_TESTS
    BENCHMARK_ENABLE_EXCEPTIONS
    BENCHMARK_ENABLE_LTO
    BENCHMARK_USE_LIBCXX
    BENCHMARK_BUILD_32_BITS
    BENCHMARK_ENABLE_INSTALL
    BENCHMARK_DOWNLOAD_DEPENDENCIES
    BENCHMARK_ENABLE_GTEST_TESTS
    BENCHMARK_ENABLE_ASSEMBLY_TESTS
    LIBRT
)
