set(UPNPC_BUILD_STATIC ON CACHE BOOL "Build static library" FORCE)
set(UPNPC_BUILD_SHARED OFF CACHE BOOL "Build shared library" FORCE)
set(UPNPC_BUILD_TESTS OFF CACHE BOOL "Build test executables" FORCE)
set(UPNPC_BUILD_TESTS OFF CACHE BOOL "Build test executables" FORCE)
set(UPNPC_BUILD_SAMPLE OFF CACHE BOOL "Build sample executables" FORCE)

add_subdirectory(miniupnp/miniupnpc EXCLUDE_FROM_ALL)
