add_library(linenoise INTERFACE IMPORTED GLOBAL)
target_include_directories(linenoise INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/yhirose-cpp-linenoise)
