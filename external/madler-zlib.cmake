## ZLIB -- Compression Support
set(_CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
if(MSVC)
  string(REGEX REPLACE "/W[1234X]" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w")
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
endif()
set(BUILD_SHARED_LIBS OFF)
set(SKIP_INSTALL_ALL ON)
add_subdirectory(madler-zlib)
target_include_directories(
  zlibstatic

  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/madler-zlib
    ${CMAKE_CURRENT_BINARY_DIR}/madler-zlib
)
set(CMAKE_C_FLAGS ${_CMAKE_C_FLAGS})
