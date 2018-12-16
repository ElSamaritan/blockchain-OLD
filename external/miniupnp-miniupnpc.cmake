set(UPNPC_BUILD_STATIC ON CACHE BOOL "Build static library" FORCE)
set(UPNPC_BUILD_SHARED OFF CACHE BOOL "Build shared library" FORCE)
set(UPNPC_BUILD_TESTS OFF CACHE BOOL "Build test executables" FORCE)
set(UPNPC_BUILD_SAMPLE OFF CACHE BOOL "Build sample executables" FORCE)

add_subdirectory(miniupnp/miniupnpc EXCLUDE_FROM_ALL)
target_include_directories(libminiupnpc-static INTERFACE ${PROJECT_SOURCE_DIR}/external/miniupnp)

mark_as_advanced(
  UPNPC_BUILD_STATIC
  UPNPC_BUILD_SHARED
  UPNPC_BUILD_TESTS
  UPNPC_BUILD_SAMPLE
  NO_GETADDRINFO
)
