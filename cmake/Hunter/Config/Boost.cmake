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

# Remember to change Packages/Boost.cmake too
set(
  XI_BOOST_REQUIRED_COMPONENTS
    system
    filesystem
    thread
    date_time
    chrono
    regex
    serialization
    program_options
    iostreams
    random
)

set(boost_b2_args)
macro(add_boost_b2_flag flag value)
 foreach(boost_component ${XI_BOOST_REQUIRED_COMPONENTS})
   string(TOUPPER ${boost_component} boost_component_toupper)
   list(APPEND boost_b2_args "${boost_component_toupper}_${flag}=${value}")
   set(boost_b2_args "${boost_b2_args}" PARENT_SCOPE)
 endforeach()
endmacro()

add_boost_b2_flag(link static)
add_boost_b2_flag(runtime-link static)
add_boost_b2_flag(threading multi)

if(MSVC)
 list(APPEND boost_b2_args "BOOST_BUILD_DYNAMIC_VSRUNTIME=NO")
endif()

hunter_config(
 Boost

 VERSION
   "1.70.0-p0"

 KEEP_PACKAGE_SOURCES

 CMAKE_ARGS
   ${boost_b2_args}
)
