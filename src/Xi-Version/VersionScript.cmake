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

set(version_file "${PROJECT_SOURCE_DIR}/VERSION")
set(license_file "${PROJECT_SOURCE_DIR}/LICENSE")
set(third_party_file "${PROJECT_SOURCE_DIR}/THIRD_PARTY")
set(third_party_license_file "${PROJECT_SOURCE_DIR}/THIRD_PARTY_LICENSE")

file(READ ${version_file} XI_VERSION)
# Some editors append new lines at the end of the file, which can lead to errors
string(REGEX REPLACE "\n$" "" XI_VERSION "${XI_VERSION}")
string(REPLACE "." ";" VERSION_LIST ${XI_VERSION})
list(GET VERSION_LIST 0 XI_VERSION_MAJOR)
list(GET VERSION_LIST 1 XI_VERSION_MINOR)
list(GET VERSION_LIST 2 XI_VERSION_PATCH)
list(GET VERSION_LIST 3 XI_VERSION_TWEAK)
message(STATUS "[--XI--] Current Version: ${XI_VERSION}")

# Read license files
file(READ ${license_file} XI_LICENSE HEX)
string(REGEX REPLACE "(..)" "'\\\\x\\1', " XI_LICENSE ${XI_LICENSE})
file(READ ${third_party_file} XI_THIRD_PARTY HEX)
string(REGEX REPLACE "(..)" "'\\\\x\\1', " XI_THIRD_PARTY ${XI_THIRD_PARTY})
file(READ ${third_party_license_file} XI_THIRD_PARTY_LICENSE HEX)
string(REGEX REPLACE "(..)" "'\\\\x\\1', " XI_THIRD_PARTY_LICENSE ${XI_THIRD_PARTY_LICENSE})

# Evaluate git version
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

set(version_header_in_file "${XI_VERSION_SOURCE_DIR}/Version.h.in")
set(version_header_out_file "${XI_VERSION_OUT_DIR}/include/Xi/Version.h")
set(version_header_temp_file "${XI_VERSION_OUT_DIR}/Version.h")
set(version_source_in_file "${XI_VERSION_SOURCE_DIR}/Version.cpp.in")
set(version_source_out_file "${XI_VERSION_OUT_DIR}/source/Version.cpp")
set(version_source_temp_file "${XI_VERSION_OUT_DIR}/Version.cpp")

# First we configure the files into temporary files withour replacing maybe existing files
configure_file(${version_header_in_file} ${version_header_temp_file})
configure_file(${version_source_in_file} ${version_source_temp_file})

# Now we check whether the output file already exists and compare their hashes
# this way we wont modify the sources if nothing changed at all and reduce compilation time
if(EXISTS ${version_header_out_file})
    file(MD5 ${version_header_out_file} currentHash)
    file(MD5 ${version_header_temp_file} newHash)

    if(NOT currentHash STREQUAL newHash)
        file(COPY ${version_header_temp_file} DESTINATION "${XI_VERSION_OUT_DIR}/include/Xi/")
    endif()
else()
    file(COPY ${version_header_temp_file} DESTINATION "${XI_VERSION_OUT_DIR}/include/Xi/")
endif()

if(EXISTS ${version_source_out_file})
    file(MD5 ${version_source_out_file} currentHash)
    file(MD5 ${version_source_temp_file} newHash)

    if(NOT currentHash STREQUAL newHash)
        file(COPY ${version_source_temp_file} DESTINATION "${XI_VERSION_OUT_DIR}/source/")
    endif()
else()
    file(COPY ${version_source_temp_file} DESTINATION "${XI_VERSION_OUT_DIR}/source/")
endif()
