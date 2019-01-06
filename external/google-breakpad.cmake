# ============================================================================================== #
#                                                                                                #
#                                       Xi Blockchain                                            #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Galaxia Project - Xi Blockchain                                       #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018 Galaxia Project Developers                                                      #
#                                                                                                #
# This program is free software: you can redistribute it and/or modify it under the terms of the #
# GNU General Public License as published by the Free Software Foundation, either version 3 of   #
# the License, or (at your option) any later version.                                            #
#                                                                                                #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      #
# See the GNU General Public License for more details.                                           #
#                                                                                                #
# You should have received a copy of the GNU General Public License along with this program.     #
# If not, see <https://www.gnu.org/licenses/>.                                                   #
#                                                                                                #
# ============================================================================================== #

# Thanks to KandoVR
# https://github.com/KandaoVR/qt-breakpad

# external breakpad repo
set(BREAKPAD_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/google-breakpad")
set(BREAKPAD_INCLUDE_DIRS "${BREAKPAD_SOURCE_DIR}/src")

# source copied from qtbreakpad.pri
set(BREAKPAD_SRCS
    "${BREAKPAD_SOURCE_DIR}/src/common/string_conversion.cc"
    "${BREAKPAD_SOURCE_DIR}/src/common/convert_UTF.c"
    "${BREAKPAD_SOURCE_DIR}/src/common/md5.cc"
)

if (WIN32)
    set(BREAKPAD_SRCS ${BREAKPAD_SRCS}
        "${BREAKPAD_SOURCE_DIR}/src/common/windows/guid_string.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/windows/handler/exception_handler.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/windows/sender/crash_report_sender.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/windows/crash_generation/minidump_generator.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/windows/crash_generation/client_info.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/windows/crash_generation/crash_generation_client.cc"
    )
    list(APPEND BREAKPAD_INCLUDE_DIRS "${BREAKPAD_SOURCE_DIR}/src/client/windows")
endif(WIN32)

if (UNIX)
    set(BREAKPAD_SRCS ${BREAKPAD_SRCS}
        "${BREAKPAD_SOURCE_DIR}/src/client/minidump_file_writer.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/log/log.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/handler/exception_handler.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/handler/minidump_descriptor.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/linux/guid_creator.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/dump_writer_common/thread_info.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/dump_writer_common/ucontext_reader.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/minidump_writer/linux_dumper.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/minidump_writer/minidump_writer.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/minidump_writer/linux_ptrace_dumper.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/microdump_writer/microdump_writer.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/linux/file_id.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/linux/elfutils.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/linux/linux_libc_support.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/linux/memory_mapped_file.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/linux/safe_readlink.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/linux/google_crashdump_uploader.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/linux/crash_generation/crash_generation_client.cc"
    )
    list(APPEND BREAKPAD_INCLUDE_DIRS "${BREAKPAD_SOURCE_DIR}/src/client/linux")
endif(UNIX)

if (APPLE)
    set(BREAKPAD_SRCS ${BREAKPAD_SRCS}
        "${BREAKPAD_SOURCE_DIR}/src/client/minidump_file_writer.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/mac/crash_generation/crash_generation_client.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/mac/handler/exception_handler.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/mac/handler/minidump_generator.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/mac/handler/breakpad_nlist_64.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/mac/handler/dynamic_images.cc"
        "${BREAKPAD_SOURCE_DIR}/src/client/mac/handler/protected_memory_allocator.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/mac/bootstrap_compat.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/mac/file_id.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/mac/macho_id.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/mac/macho_reader.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/mac/macho_utilities.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/mac/macho_walker.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/mac/string_utilities.cc"
        "${BREAKPAD_SOURCE_DIR}/src/common/mac/MachIPC.mm"
    )
endif(APPLE)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set_source_files_properties(${BREAKPAD_SRCS} PROPERTIES COMPILE_FLAGS "-w")
endif() # GNU

# target library
set_source_files_properties(${BREAKPAD_SRCS} PROPERTIES COMPILE_DEFINITIONS "UNICODE")
add_library (gbreakpad ${BREAKPAD_SRCS})
target_include_directories(gbreakpad PUBLIC ${BREAKPAD_INCLUDE_DIRS})

if(WIN32)
  target_link_libraries(gbreakpad INTERFACE wininet)
endif() # WIN32
