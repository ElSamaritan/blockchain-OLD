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

option(XI_CACHE_USE "Uses a github based cache for exteranl hunter packages" ON)
option(XI_CACHE_UPLOAD "Uploads newly built external hunter packages to the github cache" OFF)

set(XI_CMAKE_HUNTER_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")
set(HUNTER_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(HUNTER_CONFIGURATION_TYPES "Debug" CACHE INTERNAL "")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(HUNTER_CONFIGURATION_TYPES "Debug" CACHE INTERNAL "")
else()
  set(HUNTER_CONFIGURATION_TYPES "Release" CACHE INTERNAL "")
endif()

if(XI_CACHE_USE)
  set(HUNTER_USE_CACHE_SERVERS "YES" CACHE INTERNAL "")
else()
  set(HUNTER_USE_CACHE_SERVERS "NO" CACHE INTERNAL "")
endif()

if(XI_CACHE_UPLOAD)
  set(HUNTER_RUN_UPLOAD YES CACHE INTERNAL "")
else()
  set(HUNTER_RUN_UPLOAD NO CACHE INTERNAL "")
endif()


set(
  HUNTER_CACHE_SERVERS
    "https://github.com/hunter-cache/hunter-cache"

  CACHE STRING "Default Cache Server"
)

set(
  HUNTER_PASSWORDS_PATH
    "${XI_CMAKE_HUNTER_DIR}/Passwords.cmake"
  CACHE FILEPATH "Hunter Passwords"
)

include("${XI_CMAKE_HUNTER_DIR}/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.165.tar.gz"
    SHA1 "5a73f91df5f6109c0bb1104d0c0ee423f7bece79"
    FILEPATH "${XI_CMAKE_HUNTER_DIR}/Config.cmake"
)
