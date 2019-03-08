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

# xi_make_app(
#   <dir>                                   << Source Directory
#   <name>                                  << Target Name
#   [LEGACY_INCLUDE]                        << Includes the src root directory
#   [OUTPUT_NAME <name>]                    << Specifies the output name, default TOLOWER(<dir>)
# )

function(xi_make_app dir name bin)
  set(app_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
  set(app_source_dir "${app_root_dir}/source")
  set(app_bin_dir "${app_root_dir}/bin")
  set(app_include_dirs ${app_source_dir})

  cmake_parse_arguments(XI_MAKE_APP "LEGACY_INCLUDE" "OUTPUT_NAME" "LINK_LIBRARIES" ${ARGN})

  file(GLOB_RECURSE app_source_files "${app_source_dir}/**.h" "${app_source_dir}/**.cpp")
  foreach(app_source_file ${app_source_files})
    set_source_files_properties(${app_source_file} PROPERTIES COMPILE_FLAGS "${XI_CXX_FLAGS}")
  endforeach(app_source_file)

  if(MSVC)
    list(APPEND app_include_dirs ${app_bin_dir})
    set(app_bin_files "${app_bin_dir}/icon.ico" "${app_bin_dir}/description.rc")
  endif()

  if(XI_MAKE_APP_LEGACY_INCLUDE)
    list(APPEND app_indclude_dirs "${CMAKE_SOURCE_DIR}/src")
  endif()

  string(TOLOWER ${dir} app_bin_name)
  if(XI_MAKE_APP_OUTPUT_NAME)
    set(app_bin_name ${XI_MAKE_APP_OUTPUT_NAME})
  endif()

  list(APPEND XI_MAKE_APP_LINK_LIBRARIES Xi.Version)
  if(WIN32)
    list(APPEND XI_MAKE_APP_LINK_LIBRARIES Rpcrt4)
  endif()

  add_executable(${name} ${app_source_files} ${app_bin_files})
  target_include_directories(${name} PRIVATE ${app_include_dirs})
  target_link_libraries(${name} PRIVATE ${XI_MAKE_APP_LINK_LIBRARIES})
  set_property(TARGET ${name} PROPERTY OUTPUT_NAME ${app_bin_name})
  set_property(TARGET ${name} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

  install(
    TARGETS
      ${name}

    PERMISSIONS
      OWNER_EXECUTE OWNER_WRITE OWNER_READ
      GROUP_EXECUTE GROUP_WRITE GROUP_READ
      WORLD_EXECUTE WORLD_WRITE WORLD_READ

    RUNTIME DESTINATION
      bin
  )
endfunction(xi_make_app)
