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

set(BUILD_GMOCK ON CACHE BOOL "Xi uses gmock for the internal test suite" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Install targets for google-test disabled" FORCE)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undef")
endif()

add_subdirectory(google-test)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${_CMAKE_CXX_FLAGS}")
endif()

set_target_properties(gtest PROPERTIES FOLDER external)
set_target_properties(gtest_main PROPERTIES FOLDER external)
set_target_properties(gmock PROPERTIES FOLDER external)
set_target_properties(gmock_main PROPERTIES FOLDER external)

mark_as_advanced(
    BUILD_GMOCK
    INSTALL_GTEST
    gtest_build_samples
    gtest_build_tests
    gtest_disable_pthreads
    gtest_force_shared_crt
    BUILD_SHARED_LIBS
)
