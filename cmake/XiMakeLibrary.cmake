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

include(CMakeParseArguments)

macro(xi_make_library source_dir)
  string(REPLACE "-" "." lib_name ${source_dir})
  set(lib_include_dir "${source_dir}/include")
  set(lib_source_dir "${source_dir}/source")
  file(GLOB_RECURSE public_header_files "${lib_include_dir}/**.h")
  file(GLOB_RECURSE private_header_files "${lib_source_dir}/**.h")
  file(GLOB_RECURSE source_files "${lib_source_dir}/**.cpp")

  cmake_parse_arguments(XI_MAKE_LIBRARY "" "" "PUBLIC_LIBRARIES;PRIVATE_LIBRARIES;TEST_LIBRARIES" ${ARGN})

  foreach(source_file ${public_header_files} ${private_hader_files} ${source_files})
    set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "${XI_CXX_FLAGS}")
    source_group("" FILES ${source_file})
  endforeach()

  if(source_files)
    add_library(
      ${lib_name}

      STATIC
        ${public_header_files}
        ${private_header_files}
        ${source_files}
    )

    target_include_directories(
      ${lib_name}

      PUBLIC
        ${lib_include_dir}

      PRIVATE
        ${lib_source_dir}
    )
    target_link_libraries(
      ${lib_name}

      PUBLIC
        ${XI_MAKE_LIBRARY_PUBLIC_LIBRARIES}

      PRIVATE
        ${XI_MAKE_LIBRARY_PRIVATE_LIBRARIES}
    )
  else()
    add_library(${lib_name} INTERFACE)
    target_sources(${lib_name} INTERFACE ${public_header_files})
    target_include_directories(${lib_name} INTERFACE ${lib_include_dir})
  endif()

  if(XI_BUILD_TESTSUITE)
    file(GLOB_RECURSE lib_test_files "${source_dir}/tests/**.h" "${source_dir}/tests/**.cpp")
    if(lib_test_files)
      set(lib_test_name "${lib_name}.UnitTests")
      add_executable(${lib_test_name} ${lib_test_files})
      foreach(source_file ${lib_test_files})
        set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "${XI_CXX_FLAGS}")
        source_group("" FILES ${source_file})
      endforeach()
      target_link_libraries(
        ${lib_test_name}

        PRIVATE
          ${lib_name}
          ${XI_MAKE_LIBRARY_TEST_LIBRARIES}
          gmock_main
      )
      target_include_directories(
        ${lib_test_name}

        PRIVATE
          "${source_dir}/tests"
      )
      add_test(${lib_test_name} ${lib_test_name})
    endif()
  endif()
endmacro(xi_make_library)
