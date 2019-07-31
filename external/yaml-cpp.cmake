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

if(DEFINED XI_CMAKE_EXTERNAL_YAML_CPP)
  return()
endif()

set(XI_CMAKE_EXTERNAL_YAML_CPP ON)

if(MSVC)
  set(
    YAMLCPP_EXTRA_CMAKE_ARGS
      -DMSVC_SHARED_RT=OFF
      -DMSVC_STHREADED_RT=OFF
  )
endif()

ExternalProject_Add(
  jbeder-yaml-cpp

  PREFIX "jbeder-yaml-cpp"

  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  TEST_COMMAND ""

  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp"
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp

  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp
    -DYAML_CPP_BUILD_TOOLS=OFF
    -DYAML_CPP_BUILD_TESTS=OFF
    -DYAML_CPP_INSTALL=ON
    ${YAMLCPP_EXTRA_CMAKE_ARGS}
)

install()
ExternalProject_Get_Property(jbeder-yaml-cpp INSTALL_DIR)

add_library(yaml-cpp STATIC IMPORTED GLOBAL)
add_dependencies(yaml-cpp jbeder-yaml-cpp)
add_library(yaml::yaml-cpp ALIAS yaml-cpp)

include(GNUInstallDirs)
make_directory("${INSTALL_DIR}/include")
set(YAML_CPP_LIB_DIR ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR})
if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_property(TARGET yaml-cpp PROPERTY IMPORTED_LOCATION ${YAML_CPP_LIB_DIR}/libyaml-cppmtd.lib)
    else()
        set_property(TARGET yaml-cpp PROPERTY IMPORTED_LOCATION ${YAML_CPP_LIB_DIR}/libyaml-cppmt.lib)
    endif()
else()
    set_property(TARGET yaml-cpp PROPERTY IMPORTED_LOCATION ${YAML_CPP_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}yaml-cpp${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()
set_property(TARGET yaml-cpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}/include")
