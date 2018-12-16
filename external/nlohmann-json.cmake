add_library(json INTERFACE IMPORTED GLOBAL)
target_include_directories(json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann-json/include)
