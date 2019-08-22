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

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

set(CMAKE_C_STANDARD 11 CACHE INTERNAL "")
set(CMAKE_C_STANDARD_REQUIRED ON CACHE INTERNAL "")
set(CMAKE_C_EXTENSIONS OFF CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 17 CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD_REQUIRED YES CACHE INTERNAL "")
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "")

set(CMAKE_C_FLAGS_DEBUG "-m64 -g -D_DEBUG" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE "-m64 -O3 -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-m64 -O2 -g -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_MINSIZEREL "-m64 -Os -DNDEBUG" CACHE INTERNAL "")

set(CMAKE_CXX_FLAGS_DEBUG "-m64 -g -D_DEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE "-m64 -O3 -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-m64 -O2 -g -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-m64 -Os -DNDEBUG" CACHE INTERNAL "")

set(CMAKE_EXEC_LINKER_FLAGS_DEBUG "-static-libgcc -static-libstdc++")
set(CMAKE_EXEC_LINKER_FLAGS_RELEASE "-static-libgcc -static-libstdc++")
set(CMAKE_EXEC_LINKER_FLAGS_RELWITHDEBINFO "-static-libgcc -static-libstdc++")
set(CMAKE_EXEC_LINKER_FLAGS_MINSIZEREL "-static-libgcc -static-libstdc++")