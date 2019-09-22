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

if(DEFINED XI_EXTERNAL_URIPARSER_URIPARSER_CMAKE)
    return()
endif()
set(XI_EXTERNAL_URIPARSER_URIPARSER_CMAKE ON)

set(URIPARSER_BUILD_CHAR ON CACHE INTERNAL "" FORCE)
set(URIPARSER_BUILD_WCHAR_T OFF CACHE INTERNAL "" FORCE)
set(URIPARSER_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
set(URIPARSER_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(URIPARSER_BUILD_TOOLS OFF CACHE INTERNAL "" FORCE)

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(URIPARSER_MSVC_RUNTIME "/MTd" CACHE INTERNAL "" FORCE)
    else()
        set(URIPARSER_MSVC_RUNTIME "/MT" CACHE INTERNAL "" FORCE)
    endif()
endif()

add_subdirectory(uriparser)
add_library(uriparser::uriparser ALIAS uriparser)
