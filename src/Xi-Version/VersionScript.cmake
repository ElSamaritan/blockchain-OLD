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

# This file is attended to be invoked by a custom target ${CMAKE_COMMAND}
# conditionally updating the version definition.

if(XI_SKIP_VERSION_UPDATE)
  return()
endif()

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
message(STATUS "[--XI--] Current Version: ${XI_VERSION}")

# Build Channel
string(TOUPPER ${XI_BUILD_CHANNEL} XI_BUILD_CHANNEL_ID)

# Timestamp
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%dT%H:%M:%SZ" UTC)

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

function(xi_make_version_info_file file)
  set(file_in "${XI_VERSION_SOURCE_DIR}/${file}.in")
  set(file_temp "${XI_VERSION_OUT_DIR}/${file}")

  get_filename_component(file_ext ${file} EXT)
  if(file_ext STREQUAL ".cpp")
    set(file_odir "${XI_VERSION_OUT_DIR}/source")
  else()
    set(file_odir "${XI_VERSION_OUT_DIR}/include/Xi/Version")
  endif()
  set(file_out "${file_odir}/${file}")

  configure_file(${file_in} ${file_temp})

  if(EXISTS ${file_out})
      file(MD5 ${file_out} currentHash)
      file(MD5 ${file_temp} newHash)

      if(NOT currentHash STREQUAL newHash)
          file(COPY ${file_temp} DESTINATION "${file_odir}")
      endif()
  else()
      file(COPY ${file_temp} DESTINATION "${file_odir}")
  endif()
endfunction()

file(GLOB XI_VERSION_INPUT_FILES RELATIVE ${XI_VERSION_SOURCE_DIR} "${XI_VERSION_SOURCE_DIR}/*.in")
foreach(input_file ${XI_VERSION_INPUT_FILES})
  string(REPLACE ".in" "" input_file ${input_file})
  xi_make_version_info_file(${input_file})
endforeach()
