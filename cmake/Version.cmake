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
set(XI_VERSION_INPUT_FILE "${PROJECT_SOURCE_DIR}/VERSION")
set(XI_THIRD_PARTY_INPUT_FILE "${PROJECT_SOURCE_DIR}/THIRD_PARTY")
set(XI_VERSION_INCLUDE_DIR "${CMAKE_BINARY_DIR}/version")
set(XI_VERSION_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/src/config/version.h.in")
set(XI_VERSION_FILE "${XI_VERSION_INCLUDE_DIR}/version.h")
file(MAKE_DIRECTORY ${XI_VERSION_INCLUDE_DIR})
add_custom_target(
    Version
    COMMAND ${CMAKE_COMMAND}
        -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
        -DXI_VERSION_INPUT_FILE=${XI_VERSION_INPUT_FILE}
        -DXI_THIRD_PARTY_INPUT_FILE=${XI_THIRD_PARTY_INPUT_FILE}
        -DXI_VERSION_TEMPLATE=${XI_VERSION_TEMPLATE}
        -DXI_VERSION_INCLUDE_DIR=${XI_VERSION_INCLUDE_DIR}
        -P ${versionScript}
    BYPRODUCT ${XI_VERSION_FILE}
    COMMENT "Updating version information..."
    SOURCES ${XI_VERSION_TEMPLATE} ${XI_VERSION_INPUT_FILE} ${versionScript}
)
