add_library(leathers INTERFACE IMPORTED GLOBAL)
target_include_directories(leathers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ruslo-leathers/Source)
