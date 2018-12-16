﻿## ZLIB -- Compression Support
find_package(OpenSSL QUIET)
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w")
endif()
set(BUILD_SHARED_LIBS OFF)
set(SKIP_INSTALL_ALL ON)
add_subdirectory(madler-zlib)
set(CMAKE_C_FLAGS ${_CMAKE_C_FLAGS})
