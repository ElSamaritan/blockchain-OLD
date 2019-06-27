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

include(ExternalProject)

# Contained in buildsystem using submodules
include(fmtlib-fmt.cmake)
include(lz4-lz4.cmake)
include(facebook-rocksdb.cmake)
include(google-sparsehash-c11.cmake)
include(miniupnp-miniupnpc.cmake)
include(nlohmann-json.cmake)
include(yhirose-cpp-linenoise.cmake)
include(jarro2783-cxxopts.cmake)
include(ruslo-leathers.cmake)
include(google-cpu-features.cmake)

if(XI_BUILD_BREAKPAD)
  include(google-breakpad.cmake)
endif()

if(XI_BUILD_TESTSUITE)
  include(google-test.cmake)
  include(google-benchmark.cmake)
endif()
