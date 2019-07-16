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

set(CONFIG_ROOT_PATH ${CMAKE_SOURCE_DIR}/config)
set(CONFIG_TEMPLATE ${CMAKE_CURRENT_LIST_DIR}/Template)
set(CONFIG_MAKE_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/MakeConfig.cmake)

file(GLOB config_files ${CONFIG_ROOT_PATH}/*.json)
set(CONFIG_OUT ${CMAKE_CURRENT_BINARY_DIR}/config)

xi_status("Configurations" ${config_files})

set_source_files_properties(
  ${CONFIG_TEMPLATE}.hpp
  ${CONFIG_TEMPLATE}.cpp

  PROPERTIES
    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/Xi-Config/include
)

set(config_sources)
set(config_include "#pragma once\n#include<vector>\n")
set(config_init "namespace XiEmbeddedConfigs {
  static inline std::vector<int> init() {
  static bool _ = false\; if(_) return {}\; _ = true\;
  return {{\n")
foreach(config_file ${config_files})
  get_filename_component(config_name ${config_file} NAME_WLE)
  string(REPLACE "." "_" config_id ${config_name})

  set(config_build_dir ${CONFIG_OUT}/${config_name})
  set(config_out ${config_build_dir}/${config_name})
  set(config_hpp_out ${config_out}.hpp)
  set(config_cpp_out ${config_out}.cpp)
  file(MAKE_DIRECTORY ${config_build_dir})

  set(config_command_name Xi.Config.${config_name}.Generate)
  set(config_target_name Xi.Config.${config_name})

  add_custom_command(
    OUTPUT ${config_cpp_out} ${config_hpp_out}

    COMMAND ${CMAKE_COMMAND}
      -DCONFIG_NAME=${config_name}
      -DCONFIG_ID=${config_id}
      -DINPUT_FILE=${config_file}
      -DTEMPLATE_FILE=${CONFIG_TEMPLATE}
      -DOUTPUT_FILE=${config_out}
      -P ${CONFIG_MAKE_SCRIPT}

    DEPENDS ${CONFIG_MAKE_SCRIPT} ${CONFIG_TEMPLATE}.hpp ${CONFIG_TEMPLATE}.cpp ${config_file}
    COMMENT "Updating embedded '${config_name}' config file."
  )

  add_custom_target(
    ${config_target_name}

    SOURCES
      ${CONFIG_MAKE_SCRIPT}
      ${CONFIG_TEMPLATE}.hpp
      ${CONFIG_TEMPLATE}.cpp
      ${config_file}
      ${config_hpp_out}
      ${config_cpp_out}
  )

  list(APPEND config_sources ${config_cpp_out})
  set(config_include "${config_include}#include \"${config_name}/${config_name}.hpp\"\n")
  set(config_init "${config_init}${config_id}::init(),\n")
endforeach()

set(config_init "${config_init}}}\;}}\n")
set(config_include "${config_include}${config_init}")

file(WRITE ${CONFIG_OUT}/AllConfigs.hpp ${config_include})
