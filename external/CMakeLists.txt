include(ExternalProject)

include(pol51-openssl-cmake.cmake)
include(lz4-lz4.cmake)
include(facebook-rocksdb.cmake)
include(miniupnp-miniupnpc.cmake)

if(XI_BUILD_TESTSUITE)
  include(google-test.cmake)
  include(google-benchmark.cmake)
endif()

mark_as_advanced(
    UPNPC_BUILD_SHARED
    UPNPC_BUILD_STATIC
    UPNPC_BUILD_TESTS
)

set(
  XI_EXTERNAL_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/external/nlohmann-json/include
    ${PROJECT_SOURCE_DIR}/external/yhirose-cpp-linenoise
    ${PROJECT_SOURCE_DIR}/external/jarro2783-cxxopts/include
    ${PROJECT_SOURCE_DIR}/external/ruslo-leathers/Source
    ${PROJECT_SOURCE_DIR}/external/miniupnp                   # minpunp fails to setup includes correctly
  PARENT_SCOPE
)
