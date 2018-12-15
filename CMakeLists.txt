cmake_minimum_required(VERSION 3.9.2)

include(cmake/ApplyDefaultBuildType.cmake)

project(
  Xi
  
  DESCRIPTION
    "Xi brings you a truly private cryptocurrency you can trust"

  LANGUAGES 
    C 
    CXX
)

## This section describes our general CMake setup options
set(CMAKE_SUPPRESS_REGENERATION ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

## Options for configuring build system
option(XI_BUILD_EXECUTABLES "Builds the executables for this project (daemon, wallet...)." ON)
option(XI_BUILD_TESTSUITE "Builds the xi test suite, see ./tests" OFF)

include(cmake/CompilerCheck.cmake)
include(cmake/CompilerConfiguration.cmake)
include(cmake/Dependencies.cmake)
include(cmake/Platform.cmake)
include(cmake/Version.cmake)
include(cmake/XiMakeLibrary.cmake)

### Enable test integration
if(XI_BUILD_TESTSUITE)
  enable_testing()
endif() # XI_BUILD_TESTSUITE

add_subdirectory(external)
add_subdirectory(src)

if(XI_BUILD_TESTSUITE)
  add_subdirectory(tests)
endif() # XI_BUILD_TESTSUITE
