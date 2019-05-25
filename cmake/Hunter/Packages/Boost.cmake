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

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

if(MSVC)
  set(Boost_USE_STATIC_RUNTIME ON)
else()
  # For some reason hunter does not link static using gcc on linux
  # therefore it is disabled here even that we add runtime-link=static
  # later. If whenever boost is not found on a linux distro make sure
  # to set Boost_DEBUG before the find_package directive.
  set(Boost_USE_STATIC_RUNTIME OFF)
endif()

if(XI_COMPILER_DEBUG)
  set(Boost_USE_DEBUG_LIBS ON CACHE INTERNAL "")
  set(Boost_USE_DEBUG_RUNTIME ON CACHE INTERNAL "")
  set(Boost_USE_RELEASE_LIBS OFF CACHE INTERNAL "")
else()
  set(Boost_USE_DEBUG_LIBS OFF CACHE INTERNAL "")
  set(Boost_USE_DEBUG_RUNTIME OFF CACHE INTERNAL "")
  set(Boost_USE_RELEASE_LIBS ON CACHE INTERNAL "")
endif()

# Remember to change Config/Boost.cmake too
set(
  XI_BOOST_REQUIRED_COMPONENTS
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

hunter_add_package(
  Boost

  COMPONENTS
    ${XI_BOOST_REQUIRED_COMPONENTS}
)

find_package(
  Boost

  REQUIRED COMPONENTS
    ${XI_BOOST_REQUIRED_COMPONENTS}
)

mark_as_advanced(BZip2_DIR)

if(XI_CXX_COMPILER_GNU)
  list(APPEND Boost_LIBRARIES rt zlib)
elseif(APPLE)
  list(APPEND Boost_LIBRARIES zlib)
endif()

add_library(boost INTERFACE IMPORTED GLOBAL)
foreach(boost_component ${XI_BOOST_REQUIRED_COMPONENTS})
  target_link_libraries(boost INTERFACE "Boost::${boost_component}")
endforeach()

if(MSVC)
  target_compile_definitions(boost INTERFACE _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
endif()

set(Boost_VERSION ${Boost_VERSION} CACHE INTERNAL "Boost Version" FORCE)
mark_as_advanced(Boost_VERSION)
