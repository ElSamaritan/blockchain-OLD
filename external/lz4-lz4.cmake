if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(LZ4_BUILD_TYPE "Debug")
else()
  set(LZ4_BUILD_TYPE "Release")
endif()

ExternalProject_Add(
  lz4-lz4

  PREFIX "lz4-lz4"

  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  TEST_COMMAND ""

  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lz4-lz4"
  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/lz4-install

  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/lz4-install
    -DCMAKE_BUILD_TYPE=${LZ4_BUILD_TYPE}
)
install()
ExternalProject_Get_Property(lz4-lz4 INSTALL_DIR)

add_library(lz4 STATIC IMPORTED GLOBAL)
add_dependencies(lz4 lz4-lz4)

set_property(TARGET lz4 PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/lib/lz4${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET lz4 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}/include")
set(LZ4_ROOT_DIR "${INSTALL_DIR}" CACHE PATH "External project lz4 installation directory." FORCE)
mark_as_advanced(LZ4_ROOT_DIR)
