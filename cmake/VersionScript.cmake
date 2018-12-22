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

# This file is attended to be invoked by a custom target ${CMAKE_COMMAND}
# conditionally updating the version definition.

file(READ ${XI_VERSION_INPUT_FILE} XI_VERSION)
string(REGEX REPLACE "\n$" "" XI_VERSION "${XI_VERSION}")
string(REPLACE "." ";" VERSION_LIST ${XI_VERSION})
list(GET VERSION_LIST 0 XI_VERSION_MAJOR)
list(GET VERSION_LIST 1 XI_VERSION_MINOR)
list(GET VERSION_LIST 2 XI_VERSION_PATCH)
list(GET VERSION_LIST 3 XI_VERSION_TWEAK)
message(STATUS "[--XI--] Current Version: ${XI_VERSION}")

if(DEFINED ENV{CI})
    set(GIT_BRANCH $ENV{CI_COMMIT_REF_SLUG})
    set(GIT_COMMIT_HASH $ENV{CI_COMMIT_SHA})
    message(STATUS "[--XI--] CI Version:\n\tBranch:\t${GIT_BRANCH}\n\tCommit:\t${GIT_COMMIT_HASH}")
else()
    find_package(Git QUIET)
    if(Git_FOUND)
        # Get the current working branch
        execute_process(
            COMMAND git rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Get the latest abbreviated commit hash of the working branch
        execute_process(
            COMMAND git log -1 --format=%h
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        message(STATUS "[--XI--] Git Version:\n\tBranch:\t${GIT_BRANCH}\n\tCommit:\t${GIT_COMMIT_HASH}")
    else()
        set(GIT_BRANCH "NA")
        set(GIT_COMMIT_HASH "NA")
    endif()
endif() # DEFINED ENV{APPVEYOR}

set(XI_VERSION_FILE "${XI_VERSION_INCLUDE_DIR}/version.h")
set(tempfile "${CMAKE_BINARY_DIR}/version.h")
configure_file(${XI_VERSION_TEMPLATE} ${tempfile})

if(EXISTS ${XI_VERSION_FILE})
    file(MD5 ${XI_VERSION_FILE} currentHash)
    file(MD5 ${tempfile} newHash)

    if(NOT currentHash STREQUAL newHash)
        file(COPY ${tempfile} DESTINATION ${XI_VERSION_INCLUDE_DIR})
    endif()
else()
    file(COPY ${tempfile} DESTINATION ${XI_VERSION_INCLUDE_DIR})
endif()
