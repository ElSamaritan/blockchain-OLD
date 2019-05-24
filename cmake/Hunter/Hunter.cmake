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
set(HUNTER_CONFIGURATION_TYPES "Release;Debug" CACHE INTERNAL "")

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
    "${CMAKE_SOURCE_DIR}/cmake/Hunter/Passwords.cmake"
  CACHE FILEPATH "Hunter Passwords"
)

include("${CMAKE_SOURCE_DIR}/cmake/Hunter/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.174.tar.gz"
    SHA1 "2f117aa814726fe39a0aeedf84df4c1b9912389f"
    FILEPATH "${CMAKE_SOURCE_DIR}/cmake/Hunter/Config.cmake"
)
