cmake_policy(PUSH)

if(CMAKE_VERSION VERSION_GREATER "3.12")
  cmake_policy(SET CMP0074 OLD)
endif()

# Required for finding Threads on ARM
find_package(Threads)

## Go get us some static BOOST libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

if(MSVC)
  set(Boost_COMPONENTS_EXTRA zlib)
endif()

find_package(
    Boost 1.68.0

    REQUIRED COMPONENTS
        system
        filesystem
        thread
        date_time
        chrono
        regex
        serialization
        program_options
        iostreams
        ${Boost_COMPONENTS_EXTRA}
)
if(NOT MSVC)
  set(Boost_LIBRARIES "${Boost_LIBRARIES};rt")
endif()
cmake_policy(POP)

add_library(boost INTERFACE IMPORTED GLOBAL)
target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})
