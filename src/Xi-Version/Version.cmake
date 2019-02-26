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

## This section helps us tag our builds with the git commit information
set(versionScript "${CMAKE_CURRENT_LIST_DIR}/VersionScript.cmake")

set(XI_VERSION_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Xi-Version")
set(XI_VERSION_INCLUDE_DIR "${XI_VERSION_OUT_DIR}/include")
set(XI_VERSION_SOURCE_DIR "${XI_VERSION_OUT_DIR}/source")

set(XI_ALLOWED_CHANNELS "stable" "beta" "edge" "clutter")
list(FIND XI_ALLOWED_CHANNELS ${XI_BUILD_CHANNEL} XI_IS_BUILD_CHANNEL_ALLOWED)
if(XI_IS_BUILD_CHANNEL_ALLOWED EQUAL "-1")
  message(FATAL_ERROR "The XI_BUILD_CHANNEL is invalid ('${XI_BUILD_CHANNEL}')")
endif()

file(GLOB XI_VERSION_INPUT_FILES "${CMAKE_CURRENT_LIST_DIR}/*.in")

set(XI_VERSION_OUTPUT_FILES)
foreach(input_file ${XI_VERSION_INPUT_FILES})
  get_filename_component(input_file_ext ${input_file} EXT)
  get_filename_component(output_file ${input_file} NAME)
  string(REPLACE ".in" "" output_file ${output_file})
  if(input_file_ext STREQUAL ".cpp")
    list(APPEND XI_VERSION_OUTPUT_FILES "${XI_VERSION_INCLUDE_DIR}/Xi/Version/${output_file}")
  else()
    list(APPEND XI_VERSION_OUTPUT_FILES "${XI_VERSION_SOURCE_DIR}/${output_file}")
  endif()
endforeach() # input_file

list(
  APPEND

  XI_VERSION_INPUT_FILES
    "${PROJECT_SOURCE_DIR}/VERSION"
    "${PROJECT_SOURCE_DIR}/LICENSE"
    "${PROJECT_SOURCE_DIR}/THIRD_PARTY"
    "${PROJECT_SOURCE_DIR}/THIRD_PARTY_LICENSE"
    ${versionScript}
)

add_custom_target(
  Xi.Version.Generate
  COMMAND ${CMAKE_COMMAND}
      -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
      -DXI_VERSION_OUT_DIR=${XI_VERSION_OUT_DIR}
      -DXI_VERSION_SOURCE_DIR=${CMAKE_CURRENT_LIST_DIR}
      -DXI_SKIP_VERSION_UPDATE=${XI_SKIP_VERSION_UPDATE}
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_C_COMPILER_ID=${CMAKE_C_COMPILER_ID}
      -DCMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}
      -DCMAKE_C_COMPILER_VERSION=${CMAKE_C_COMPILER_VERSION}
      -DCMAKE_CXX_COMPILER_VERSION=${CMAKE_CXX_COMPILER_VERSION}
      -DXI_BUILD_CHANNEL=${XI_BUILD_CHANNEL}
      -DOPENSSL_VERSION=${OPENSSL_VERSION}
      -DBoost_VERSION=${Boost_VERSION}
      -P ${versionScript}
  BYPRODUCTS ${XI_VERSION_OUTPUT_FILES}
  COMMENT "Updating version information..."
)

file(
  MAKE_DIRECTORY
    ${XI_VERSION_INCLUDE_DIR}/Xi/Version
    ${XI_VERSION_SOURCE_DIR}
)

set_source_files_properties(${XI_VERSION_INPUT_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
add_library(Xi.Version STATIC ${XI_VERSION_OUTPUT_FILES} ${XI_VERSION_INPUT_FILES})
target_include_directories(Xi.Version PUBLIC ${XI_VERSION_INCLUDE_DIR})
add_dependencies(Xi.Version Xi.Version.Generate)
