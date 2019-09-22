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

if(DEFINED XI_SRC_XI_RESOURCE_BUILDER_RESOURCE_BUILDER_CMAKE)
  return()
endif()
set(XI_SRC_XI_RESOURCE_BUILDER_RESOURCE_BUILDER_CMAKE ON)

set(XI_RESOURCE_BUILDER_ROOT ${CMAKE_CURRENT_LIST_DIR})

add_executable(
  Xi.Resource.Builder
  ${XI_RESOURCE_BUILDER_ROOT}/ResourceBuilder.cpp
)

target_link_libraries(
  Xi.Resource.Builder

  PRIVATE
    Xi::Core
    Xi::Crypto
    Xi::Resource

    boost::boost
    cxxopts::cxxopts
)

include(CMakeParseArguments)

function(xi_make_resources)
  cmake_parse_arguments(
    XI_MAKE_RESOURCES
      ""
      "LIBRARY_NAME;NAMESPACE;OUTPATH"
      "INPUTS"

    ${ARGN}
  )

  set(rootDir ${XI_MAKE_RESOURCES_OUTPATH}/Resources)
  set(includeDir ${rootDir}/include)
  set(headersDir ${includeDir})
  set(sourceDir ${rootDir}/source)


  string(REPLACE ":" ";" nspath ${XI_MAKE_RESOURCES_NAMESPACE})
  foreach(ns ${nspath})
    set(headersDir "${headersDir}/${ns}")
  endforeach()

  file(MAKE_DIRECTORY ${includeDir})
  file(MAKE_DIRECTORY ${sourceDir})

  set(headerFiles)
  set(sourceFiles)
  foreach(input ${XI_MAKE_RESOURCES_INPUTS})
    get_filename_component(inputName ${input} NAME_WE)

    set(inputHeaderFile "${headersDir}/${inputName}.hpp")
    set(inputSourceFile "${sourceDir}/${inputName}.cpp")

    list(APPEND headerFiles ${inputHeaderFile})
    list(APPEND sourceFiles ${inputSourceFile})

    string(RANDOM LENGTH 16 proxyFile)
    add_custom_command(
      OUTPUT
        ${proxyFile}

      COMMAND ""
    )

    add_custom_command(
      OUTPUT
        ${inputHeaderFile}
        ${inputSourceFile}

      COMMAND
        Xi.Resource.Builder
          --inputs ${input}
          --output ${rootDir}

      DEPENDS
        ${proxyFile}
        ${input}
        Xi.Resource.Builder

      BYPRODUCTS
        ${inputHeaderFile}
        ${inputSourceFile}
    )
  endforeach()

  add_library(
    ${XI_MAKE_RESOURCES_LIBRARY_NAME}

    STATIC
      ${headerFiles}
      ${sourceFiles}
  )

  target_link_libraries(
    ${XI_MAKE_RESOURCES_LIBRARY_NAME}

    PRIVATE
      Xi::Resource
  )

  target_include_directories(
    ${XI_MAKE_RESOURCES_LIBRARY_NAME}

    PUBLIC
      ${includeDir}
  )

  target_sources(
    ${XI_MAKE_RESOURCES_LIBRARY_NAME}

    PRIVATE
      ${XI_MAKE_RESOURCES_INPUTS}
  )

  string(REPLACE "." "::" alias ${XI_MAKE_RESOURCES_LIBRARY_NAME})
  add_library(${alias} ALIAS ${XI_MAKE_RESOURCES_LIBRARY_NAME})
endfunction() # xi_make_resources
