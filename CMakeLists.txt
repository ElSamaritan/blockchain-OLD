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

cmake_minimum_required(VERSION 3.12.4)

include(cmake/ApplyDefaultBuildType.cmake)

project(
  Xi
  
  DESCRIPTION
    "Xi brings you a truly private cryptocurrency you can trust"

  LANGUAGES 
    C 
    CXX
)

## This section describes our general CMake setup options
set(CMAKE_SUPPRESS_REGENERATION ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

## Options for configuring build system
set(XI_BUILD_CHANNEL "clutter" CACHE STRING "The distributation channel this build is used for. [stable|beta|edge|clutter]")
option(XI_BUILD_EXECUTABLES "Builds the executables for this project (daemon, wallet...)." ON)
option(XI_BUILD_BREAKPAD "Includes the breakpad crash dumper. More options will be availabe it turned on." OFF)
option(XI_BUILD_TESTSUITE "Builds the xi test suite, see ./tests" OFF)
option(XI_BUILD_TOOLS "Builds tooling executables used in development" OFF)
option(XI_SKIP_VERSION_UPDATE "Skips version update to reduce compile time while developing" OFF)

include(cmake/CompilerCheck.cmake)
include(cmake/CompilerConfiguration.cmake)
include(cmake/Platform.cmake)
include(cmake/XiMakeLibrary.cmake)
include(cmake/XiMakeApp.cmake)

### Enable test integration
if(XI_BUILD_TESTSUITE)
  enable_testing()
endif() # XI_BUILD_TESTSUITE

add_subdirectory(external)
add_subdirectory(src)

if(XI_BUILD_EXECUTABLES)
  add_subdirectory(app)
endif() # XI_BUILD_EXECUTABLES

if(XI_BUILD_TOOLS)
  add_subdirectory(tools)
endif() # XI_BUILD_TOOLS

if(XI_BUILD_TESTSUITE)
  add_subdirectory(tests)
endif() # XI_BUILD_TESTSUITE
