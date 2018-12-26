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

## This section helps us tag our builds with the git commit information
set(versionScript "${CMAKE_CURRENT_LIST_DIR}/VersionScript.cmake")

set(XI_VERSION_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Xi-Version")
set(XI_VERSION_INCLUDE_DIR "${XI_VERSION_OUT_DIR}/include")
set(XI_VERSION_SOURCE_DIR "${XI_VERSION_OUT_DIR}/source")

set(
  XI_VERSION_INPUT_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Version.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/Version.cpp.in"
    "${PROJECT_SOURCE_DIR}/LICENSE"
    "${PROJECT_SOURCE_DIR}/THIRD_PARTY"
    "${PROJECT_SOURCE_DIR}/THIRD_PARTY_LICENSE"
)

set(
  XI_VERSION_OUTPUT_FILES
    "${XI_VERSION_INCLUDE_DIR}/Xi/Version.h"
    "${XI_VERSION_SOURCE_DIR}/Version.cpp"
)

file(
  MAKE_DIRECTORY
    ${XI_VERSION_INCLUDE_DIR}/Xi
    ${XI_VERSION_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${XI_VERSION_OUTPUT_FILES}
    COMMAND ${CMAKE_COMMAND}
        -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
        -DXI_VERSION_OUT_DIR=${XI_VERSION_OUT_DIR}
        -DXI_VERSION_SOURCE_DIR=${CMAKE_CURRENT_LIST_DIR}
        -P ${versionScript}
    COMMENT "Updating version information..."
    DEPENDS ${XI_VERSION_INPUT_FILES} ${versionScript}
)

set_source_files_properties(${XI_VERSION_INPUT_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
add_library(Xi.Version STATIC ${XI_VERSION_OUTPUT_FILES} ${XI_VERSION_INPUT_FILES})
target_include_directories(Xi.Version PUBLIC ${XI_VERSION_INCLUDE_DIR})
