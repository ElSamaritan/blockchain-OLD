﻿cmake_policy(PUSH)
if(CMAKE_VERSION VERSION_GREATER "3.12")
  cmake_policy(SET CMP0074 OLD)
endif()
find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
cmake_policy(POP)

add_library(openssl INTERFACE IMPORTED GLOBAL)
target_include_directories(openssl INTERFACE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(openssl INTERFACE ${OPENSSL_LIBRARIES})
