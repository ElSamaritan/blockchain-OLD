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

if(MSVC)
    set(ROCKSDB_LIB rocksdb)
else()
    set(ROCKSDB_LIB rocksdblib)
endif()

# unit tests
file(GLOB_RECURSE XI_UNITTESTS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/unittests/*.cpp")
source_group("" FILES ${XI_UNITTESTS_SOURCE_FILES})
add_executable(TestSuite.UnitTests ${XI_UNITTESTS_SOURCE_FILES})
target_link_libraries(TestSuite.UnitTests PRIVATE gtest_main Common Crypto CryptoNoteCore Serialization Logging ${ROCKSDB_LIB})
add_test(Unit-Tests TestSuite.UnitTests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# benchmarks
file(GLOB_RECURSE XI_BENCHMARK_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/*")
source_group("" FILES ${XI_BENCHMARK_SOURCE_FILES})
add_executable(TestSuite.Benchmark ${XI_BENCHMARK_SOURCE_FILES})
target_link_libraries(TestSuite.Benchmark PRIVATE benchmark_main Common Crypto CryptoNoteCore Serialization Logging ${ROCKSDB_LIB})
target_include_directories(TestSuite.Benchmark PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/benchmark")

# tools
file(GLOB XI_TOOLS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tools/*.cpp")
source_group("" FILES ${XI_TOOLS_SOURCE_FILES})
foreach(xiTool ${XI_TOOLS_SOURCE_FILES})
  get_filename_component(xiToolName ${xiTool} NAME_WE)
  set(xiToolTarget "TestSuite.Tools.${xiToolName}")
  add_executable(${xiToolTarget} ${xiTool})
  target_link_libraries(${xiToolTarget} PRIVATE Common Crypto CryptoNoteCore Serialization Logging ${ROCKSDB_LIB})
endforeach() # xiTool
