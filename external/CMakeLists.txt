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
