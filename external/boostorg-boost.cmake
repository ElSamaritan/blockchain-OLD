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

cmake_policy(PUSH)

if(CMAKE_VERSION VERSION_GREATER "3.12")
  cmake_policy(SET CMP0074 OLD)
endif()

# Required for finding Threads on ARM
find_package(Threads)

## Go get us some static BOOST libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

set(Boost_COMPONENTS
    system
    filesystem
    thread
    date_time
    chrono
    regex
    serialization
    program_options
    iostreams
    random
)

if(MSVC)
  list(APPEND Boost_COMPONENTS zlib)
else()
  list(APPEND Boost_COMPONENTS headers)
endif()

find_package(
    Boost 1.70.0 EXACT

    REQUIRED COMPONENTS
        ${Boost_COMPONENTS}
)

if(XI_CXX_COMPILER_GNU)
  list(APPEND Boost_LIBRARIES rt zlib)
elseif(APPLE)
  list(APPEND Boost_LIBRARIES zlib)
endif()

cmake_policy(POP)

add_library(boost INTERFACE IMPORTED GLOBAL)
target_link_libraries(boost INTERFACE Boost::boost)

if(MSVC)
  target_compile_definitions(boost INTERFACE _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
endif()

set(Boost_VERSION ${Boost_VERSION} CACHE STRING "Boost Version" FORCE)
mark_as_advanced(Boost_VERSION boost_zlib_DIR)

foreach(component ${Boost_COMPONENTS})
  mark_as_advanced(boost_${component}_DIR)
  target_link_libraries(boost INTERFACE Boost::${component})
endforeach()
