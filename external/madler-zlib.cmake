## ZLIB -- Compression Support
if(NOT MSVC)
  find_package(ZLIB REQUIRED)
  add_library(zlib INTERFACE IMPORTED GLOBAL)
  target_include_directories(zlib INTERFACE ${ZLIB_INCLUDE_DIRS})
  target_link_libraries(zlib INTERFACE ${ZLIB_LIBRARIES})
endif() # NOT MSVC
