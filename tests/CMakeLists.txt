include_directories(${CMAKE_SOURCE_DIR}/external/rocksdb/include)
include_directories(${CMAKE_SOURCE_DIR}/external/linenoise)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/external)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-undef")
endif() # GCC

if(MSVC)
    set(ROCKSDB_LIB rocksdb)
else()
    set(ROCKSDB_LIB rocksdblib)
endif()

# copy test data
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/DB")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/DB" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# integration tests
file(GLOB_RECURSE XI_INTEGRATION_TESTS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/integration/*.cpp")
foreach(source_file ${XI_INTEGRATION_TESTS_SOURCE_FILES})
  if(XI_CXX_FLAGS)
    set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "${XI_CXX_FLAGS} /W3 /GS-")
  endif() # XI_CXX_FLAGS
  source_group("" FILES ${source_file})
endforeach()
add_executable(TestSuite.Integration ${XI_INTEGRATION_TESTS_SOURCE_FILES})
target_link_libraries(TestSuite.Integration PRIVATE gtest_main Common Crypto CryptoNoteCore Serialization Logging ${ROCKSDB_LIB})

add_test(Integrity TestSuite.Integration WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# benchmarks
file(GLOB_RECURSE XI_BENCHMARK_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/*")
foreach(source_file ${XI_BENCHMARK_SOURCE_FILES})
  if(XI_CXX_FLAGS)
    set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "${XI_CXX_FLAGS} /W3 /GS-")
  endif() # XI_CXX_FLAGS
  source_group("" FILES ${source_file})
endforeach()
add_executable(TestSuite.Benchmark ${XI_BENCHMARK_SOURCE_FILES})
target_link_libraries(TestSuite.Benchmark PRIVATE benchmark_main Common Crypto CryptoNoteCore Serialization Logging ${ROCKSDB_LIB})
target_include_directories(TestSuite.Benchmark PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/benchmark")
