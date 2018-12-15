## OpenSSL -- HTTPS Support
find_package(OpenSSL QUIET)
if(OPENSSL_FOUND)
  message(STATUS "[--XI--] Found OpenSSL Version ${OEPNSSL_VERSION}. You will support HTTPS encryption.")
else() # !OPENSSL_FOUND
  message(STATUS "[--XI--] Could not find any preinstalled OpenSSL Version. Going to include the openssl"
    " verison shipped with the buildsystem.")
  set(_CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
  if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w")
  endif()
  set(BUILD_SHARED_LIBS OFF)
  add_subdirectory(pol51-openssl-cmake)
  set(CMAKE_C_FLAGS ${_CMAKE_C_FLAGS})
endif() # OPENSSL_FOUND
