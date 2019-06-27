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

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

foreach(flag ${CompilerFlags})
  set(${flag} "${${flag}} ${XI_CXX_FLAGS}")
endforeach()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undef")
endif() # GCC

# unit tests
file(GLOB_RECURSE XI_UNITTESTS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/unittests/*.cpp")
source_group("" FILES ${XI_UNITTESTS_SOURCE_FILES})
add_executable(TestSuite.UnitTests ${XI_UNITTESTS_SOURCE_FILES})
target_link_libraries(TestSuite.UnitTests PRIVATE gmock_main Common Crypto CryptoNoteCore Serialization Logging rocksdb)
add_test(Unit-Tests TestSuite.UnitTests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# benchmarks
file(GLOB_RECURSE XI_BENCHMARK_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/*")
source_group("" FILES ${XI_BENCHMARK_SOURCE_FILES})
add_executable(TestSuite.Benchmark ${XI_BENCHMARK_SOURCE_FILES})
target_link_libraries(TestSuite.Benchmark PRIVATE benchmark_main Common Crypto CryptoNoteCore Serialization Logging rocksdb)
target_include_directories(TestSuite.Benchmark PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/benchmark")
