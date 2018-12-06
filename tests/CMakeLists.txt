include_directories(${CMAKE_SOURCE_DIR}/external/rocksdb/include)
include_directories(${CMAKE_SOURCE_DIR}/external/linenoise)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/external)

foreach(flag ${CompilerFlags})
  set(${flag} "${${flag}} ${XI_CXX_FLAGS}")
endforeach()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undef")
endif() # GCC

# benchmarks
file(GLOB_RECURSE XI_BENCHMARK_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/*")
source_group("" FILES ${XI_BENCHMARK_SOURCE_FILES})
add_executable(TestSuite.Benchmark ${XI_BENCHMARK_SOURCE_FILES})
target_link_libraries(TestSuite.Benchmark PRIVATE benchmark_main Common Crypto CryptoNoteCore Serialization Logging rocksdb)
target_include_directories(TestSuite.Benchmark PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/benchmark")
