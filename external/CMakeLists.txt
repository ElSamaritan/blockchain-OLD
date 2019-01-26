# ============================================================================================== #
#                                                                                                #
#                                       Xi Blockchain                                            #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Galaxia Project - Xi Blockchain                                       #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018 Galaxia Project Developers                                                      #
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

# Not contained in build system
## Required On Linux
include(madler-zlib.cmake)

## Required
include(boostorg-boost.cmake)
include(openssl-openssl.cmake)

# Contained in buildsystem using submodules
include(lz4-lz4.cmake)
include(facebook-rocksdb.cmake)
include(google-sparsehash-c11.cmake)
include(miniupnp-miniupnpc.cmake)
include(nlohmann-json.cmake)
include(yhirose-cpp-linenoise.cmake)
include(jarro2783-cxxopts.cmake)
include(ruslo-leathers.cmake)

if(XI_BREAKPAD)
  include(google-breakpad.cmake)
endif()

if(XI_BUILD_TESTSUITE)
  include(google-test.cmake)
  include(google-benchmark.cmake)
endif()
