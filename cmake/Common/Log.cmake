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

set(XI_LOG_PREFIX "[--XI--] " CACHE INTERNAL "")
set(XI_LOG_INDENT " -- " CACHE INTERNAL "")

macro(xi_log_format out_var header)
  set(${out_var} ${header})
  foreach(line ${ARGN})
    set(${out_var} "${${out_var}}\n${XI_LOG_INDENT}${line}")
  endforeach()
endmacro()

function(xi_status)
  xi_log_format(log_message ${ARGN})
  message(STATUS ${XI_LOG_PREFIX} ${log_message})
endfunction()

function(xi_warning)
  xi_log_format(log_message ${ARGN})
  message(WARNING ${XI_LOG_PREFIX} ${log_message})
endfunction()

function(xi_error)
  xi_log_format(log_message ${ARGN})
  message(SEND_ERROR ${XI_LOG_PREFIX} ${log_message})
endfunction()

function(xi_fatal)
  xi_log_format(log_message ${ARGN})
  message(FATAL_ERROR ${XI_LOG_PREFIX} ${log_message})
endfunction()
