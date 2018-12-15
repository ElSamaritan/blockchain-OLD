 # ============================================================================================== #
 #                                                                                                #
 #                                     The Galxia Project                                         #
 #                                                                                                #
 # ---------------------------------------------------------------------------------------------- #
 # This file is part of Xi                                                                        #
 # ---------------------------------------------------------------------------------------------- #
 #                                                                                                #
 # Copyright 2018 Glaxia Project Delvelopers                                                      #
 #                                                                                                #
 # Permission is hereby granted, free of charge, to any person obtaining a copy of this software  #
 # and associated documentation files (the "Software"), to deal in the Software without           #
 # restriction, including without limitation the rights to use, copy, modify, merge, publish,     #
 # distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  #
 # Software is furnished to do so, subject to the following conditions:                           #
 #                                                                                                #
 # The above copyright notice and this permission notice shall be included in all copies or       #
 # substantial portions of the Software.                                                          #
 #                                                                                                #
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  #
 # BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     #
 # NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   #
 # DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, #
 # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        #
 #                                                                                                #
 # ============================================================================================== #

include(CMakeParseArguments)

macro(xi_make_library name source_dir)
  set(lib_include_dir "${source_dir}/include")
  set(lib_source_dir "${source_dir}/source")
  file(GLOB_RECURSE public_header_files "${lib_include_dir}/**.h")
  file(GLOB_RECURSE private_header_files "${lib_source_dir}/**.h")
  file(GLOB_RECURSE source_files "${lib_source_dir}/**.cpp")

  set(lib_name "Xi.${name}")

  foreach(source_file ${public_header_files} ${private_hader_files} ${source_files})
    set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "${XI_CXX_FLAGS}")
    source_group("" FILES ${source_file})
  endforeach()

  add_library(
    ${lib_name}

    STATIC
      ${public_header_files}
      ${private_header_files}
      ${source_files}
  )

  target_include_directories(
    ${lib_name}

    PUBLIC
      ${lib_include_dir}
      ${XI_EXTERNAL_INCLUDE_DIRS}

    PRIVATE
      ${lib_source_dir}
   )

  if(XI_BUILD_TESTSUITE)
    file(GLOB_RECURSE lib_test_files "${source_dir}/tests/**.cpp")
    if(lib_test_files)
      set(lib_test_name "Xi.${name}.UnitTests")
      add_executable(${lib_test_name} ${lib_test_files})
      foreach(source_file ${lib_test_files})
        set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "${XI_CXX_FLAGS}")
        source_group("" FILES ${source_file})
      endforeach()
      target_link_libraries(
        ${lib_test_name}

        PRIVATE
          ${lib_name}
          gmock_main
      )
      add_test(${lib_test_name} ${lib_test_name})
    endif()
  endif()
endmacro(xi_make_library)
