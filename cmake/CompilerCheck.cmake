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

macro(xi_check_compiler id name min recommended)
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL ${id})
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${min})
      message(FATAL_ERROR "Your ${name} compiler is outdated (Version '${CMAKE_CXX_COMPILER_VERSION}'), at least "
        "version '${min}' is required to to compile the project.")
    elseif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${recommended})
      message(WARNING "Your ${name} compiler is outdated (Version '${CMAKE_CXX_COMPILER_VERSION}'), it is recommended "
        "to use at least version '${recommended}'.")
    endif()
    set(XI_CXX_COMPILER ${name})
    set("XI_CXX_COMPILER_${id}" ON)
  endif()
  if(${CMAKE_C_COMPILER_ID} STREQUAL ${id})
    if(CMAKE_C_COMPILER_VERSION VERSION_LESS ${min})
      message(FATAL_ERROR "Your ${name} compiler is outdated (Version '${CMAKE_C_COMPILER_VERSION}'), at least "
        "version '${min}' is required to to compile the project.")
    elseif(CMAKE_C_COMPILER_VERSION VERSION_LESS ${recommended})
      message(WARNING "Your ${name} compiler is outdated (Version '${CMAKE_C_COMPILER_VERSION}'), it is recommended "
        "to use at least version '${recommended}'.")
    endif()
  endif()
  set(XI_C_COMPILER ${name})
  set("XI_C_COMPILER_${id}" ON)
endmacro() # xi_check_compiler

xi_check_compiler("GNU" "GCC/G++" 8 8.2)
xi_check_compiler("MSVC" "Visual Studio C++" 19.10 19.15)

if(NOT XI_CXX_COMPILER)
  message(FATAL_ERROR "The C++ compiler you are using (${CMAKE_CXX_COMPILER_ID}) is currently not supported.")
endif()
if(NOT XI_C_COMPILER)
  message(FATAL_ERROR "The C compiler you are using (${CMAKE_C_COMPILER_ID}) is currently not supported.")
endif()
