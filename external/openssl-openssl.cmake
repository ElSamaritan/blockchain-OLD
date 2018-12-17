find_package(OpenSSL QUIET)
if(NOT OpenSSL_FOUND)
  message(WARNING "OpenSSL could not be found. We will migrate to it later and it will be required, you may wanna"
    " install it in advance.")
endif()
