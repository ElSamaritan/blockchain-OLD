﻿# ============================================================================================== #
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

# Enable the c++17 standard support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# We do not support 32-bit builds. When and if we do, this can be removed.
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "Targeting a 32-bit architecture is not supported.")
endif()

set(CompilerFlags
  CMAKE_CXX_FLAGS
  CMAKE_CXX_FLAGS_DEBUG
  CMAKE_CXX_FLAGS_RELEASE
  CMAKE_CXX_FLAGS_RELWITHDEBINFO
  CMAKE_CXX_FLAGS_MINSIZEREL
  CMAKE_C_FLAGS
  CMAKE_C_FLAGS_DEBUG
  CMAKE_C_FLAGS_RELEASE
  CMAKE_C_FLAGS_RELWITHDEBINFO
  CMAKE_C_FLAGS_MINSIZEREL
)

if(MSVC)
    option(XI_PARALLEL_BUILD ON "enables parallel compiler execution")
    set(XI_PARALLEL_BUILD_THREADS "-1" CACHE STRING "maximum number of threads to use for parallel compilation (<=0 -> max)")

    add_definitions("/D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x0600 /DWIN32_LEAN_AND_MEAN /DGTEST_HAS_TR1_TUPLE=0 /D_VARIADIC_MAX=8 /D__SSE4_1__")
    foreach(flag ${CompilerFlags})
        string(REGEX REPLACE "/M[TD]d?" "" ${flag} ${${flag}})
        string(REGEX REPLACE "/W[1234X]" "" ${flag} ${${flag}})
    endforeach()
    set(XI_CXX_FLAGS "/W4 /WX /bigobj /GS-")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO"${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:10485760 /ignore:4099 /ignore:4217 /ignore:4049")

    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRTD")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRT")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRT")
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /NODEFAULTLIB:MSVCRT")

    if(XI_PARALLEL_BUILD)
      if(XI_PARALLEL_BUILD_THREADS LESS_EQUAL "0")
        message(STATUS "Enabling parallel compilation with maximum threads.")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
      else()
        message(STATUS "Enabling parallel compilation with ${XI_PARALLEL_BUILD_THREADS} threads.")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP${XI_PARALLEL_BUILD_THREADS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${XI_PARALLEL_BUILD_THREADS}")
      endif()
    endif()

    if(XI_BUILD_BREAKPAD)
      set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Zi")
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
      set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /Zi")
      set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /Zi")
      string(REPLACE "/INCREMENTAL" "" CMAKE_EXE_LINKER_FLAGS_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
      string(REPLACE "/INCREMENTAL" "" CMAKE_EXE_LINKER_FLAGS_MINSIZEREL ${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL})
      set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /MAP /DEBUG:FULL /opt:ref")
      set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /MAP /DEBUG:FULL /opt:ref")
    endif() # XI_BUILD_BREAKPAD
else() # NOT MSVC
  if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    # This option has no effect in glibc version less than 2.20.
    # Since glibc 2.20 _BSD_SOURCE is deprecated, this macro is recomended instead
    add_definitions("-D_DEFAULT_SOURCE -D_GNU_SOURCE")
  endif()
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(XI_CXX_FLAGS "-Wall -Wextra -Werror")
  endif()

  ## This is here to support building for multiple architecture types... but we all know how well that usually goes...
  set(ARCH default CACHE STRING "CPU to build for: -march value or default")
  if("${ARCH}" STREQUAL "default")
    set(ARCH_FLAG "")
  else()
    set(ARCH_FLAG "-march=${ARCH}")
  endif()

  ## These options generate all those nice warnings we see while building
  set(WARNINGS "-Wall -Wextra -Wpointer-arith -Wundef -Wvla -Wwrite-strings  -Wno-error=extra -Wno-error=unused-function -Wno-error=deprecated-declarations -Wno-error=sign-compare -Wno-error=strict-aliasing -Wno-error=type-limits -Wno-unused-parameter -Wno-error=unused-variable -Wno-error=undef -Wno-error=uninitialized -Wno-error=unused-result")
  if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    set(WARNINGS "${WARNINGS} -Wno-error=mismatched-tags -Wno-error=null-conversion -Wno-overloaded-shift-op-parentheses -Wno-error=shift-count-overflow -Wno-error=tautological-constant-out-of-range-compare -Wno-error=unused-private-field -Wno-error=unneeded-internal-declaration -Wno-error=unused-function -Wno-error=missing-braces")
  else()
    set(WARNINGS "${WARNINGS} -Wlogical-op -Wno-error=maybe-uninitialized -Wno-error=clobbered -Wno-error=unused-but-set-variable")
  endif()

  set(C_WARNINGS "-Waggregate-return -Wnested-externs -Wold-style-definition -Wstrict-prototypes")
  set(CXX_WARNINGS "-Wno-reorder -Wno-missing-field-initializers")

  # We need to set the label and import it into CMake if it exists
  set(LABEL "")
  if(DEFINED ENV{LABEL})
    set(LABEL $ENV{LABEL})
    message(STATUS "Found LABEL: ${LABEL}")
  endif()
  if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64" OR "${LABEL}" STREQUAL "aarch64")
    set(MAES_FLAG "")
  elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64" AND NOT "${LABEL}" STREQUAL "aarch64")
    set(MAES_FLAG "-maes")
  endif()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${STATICASSERTC_FLAG} ${WARNINGS} ${C_WARNINGS} ${ARCH_FLAG} ${MAES_FLAG}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STATICASSERTCPP_FLAG} ${WARNINGS} ${CXX_WARNINGS} ${ARCH_FLAG} ${MAES_FLAG}")

  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_HAS_TR1_TUPLE=0")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  endif()

  ## Setting up DEBUG flags
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND NOT (CMAKE_C_COMPILER_VERSION VERSION_LESS 4.8))
    set(DEBUG_FLAGS "-g3 -Og -gdwarf-4 -fvar-tracking -fvar-tracking-assignments -fno-inline -fno-omit-frame-pointer")
  else()
    set(DEBUG_FLAGS "-g3 -O0 -fno-omit-frame-pointer")
  endif()

  ## Setting up RELEASE flags
  set(RELEASE_FLAGS "-Ofast -DNDEBUG -Wno-unused-variable")

  if(NOT APPLE)
    # There is a clang bug that does not allow to compile code that uses AES-NI intrinsics if -flto is enabled
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_SYSTEM_NAME STREQUAL "Linux"
        AND CMAKE_BUILD_TYPE STREQUAL "Release" AND ((CMAKE_C_COMPILER_VERSION VERSION_GREATER 4.9) OR (CMAKE_C_COMPILER_VERSION VERSION_EQUAL 4.9)))
      # On linux, to build in lto mode, check that ld.gold linker is used: 'update-alternatives --install /usr/bin/ld ld /usr/bin/ld.gold HIGHEST_PRIORITY'
      set(CMAKE_AR gcc-ar)
      set(CMAKE_RANLIB gcc-ranlib)
    endif()
  endif()

  ## Set up the normal CMake flags as we've built them
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${RELEASE_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS}")

  if(XI_BUILD_BREAKPAD)
    if(XI_C_COMPILER_GNU)
      set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -fno-omit-frame-pointer")
    endif()
    if(XI_CXX_COMPILER_GNU)
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -fno-omit-frame-pointer")
    endif()
  endif() # XI_BUILD_BREAKPAD

  ## Statically link our binaries
  if(NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
  endif()
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(XI_COMPILER_DEBUG ON CACHE INTERNAL "")
  set(XI_COMPILER_RELEASE OFF CACHE INTERNAL "")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(XI_COMPILER_DEBUG ON CACHE INTERNAL "")
  set(XI_COMPILER_RELEASE OFF CACHE INTERNAL "")
else()
  set(XI_COMPILER_DEBUG OFF CACHE INTERNAL "")
  set(XI_COMPILER_RELEASE ON CACHE INTERNAL "")
endif()
