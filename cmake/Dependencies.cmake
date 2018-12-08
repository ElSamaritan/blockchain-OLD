cmake_policy(PUSH)

if(CMAKE_VERSION VERSION_GREATER "3.12")
  cmake_policy(SET CMP0074 OLD)
endif()

# Required for finding Threads on ARM
find_package(Threads)

## Go get us some static BOOST libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(
    Boost 1.65.1 REQUIRED

    COMPONENTS
        system
        filesystem
        thread
        date_time
        chrono
        regex
        serialization
        program_options
)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
if(APPLE)
  set(Boost_LIBRARIES "${Boost_LIBRARIES}")
elseif(NOT MSVC)
  set(Boost_LIBRARIES "${Boost_LIBRARIES};rt")
endif()
cmake_policy(POP)
