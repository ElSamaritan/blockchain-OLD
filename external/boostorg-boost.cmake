# ============================================================================================== #
#                                                                                                #
#                                       Xi Blockchain                                            #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Galaxia Project - Xi Blockchain                                       #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018 Galaxia Project Developers                                                      #
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

if(MSVC)
  set(Boost_COMPONENTS_EXTRA zlib)
endif()

find_package(
    Boost 1.68.0

    REQUIRED COMPONENTS
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
        ${Boost_COMPONENTS_EXTRA}
)
if(NOT MSVC)
  set(Boost_LIBRARIES "${Boost_LIBRARIES};rt;zlib;boost_system")
endif()
cmake_policy(POP)

add_library(boost INTERFACE IMPORTED GLOBAL)
target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})
