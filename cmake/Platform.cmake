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

# Platform specific code base information is applied here
if(MSVC)
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows)
 include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/msc)
elseif(APPLE)
 include_directories(SYSTEM /usr/include/malloc)
 enable_language(ASM)
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/OSX)
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Posix)
else()
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Linux)
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Posix)
endif()
