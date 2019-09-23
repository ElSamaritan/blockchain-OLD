file(GLOB XI_TOOLS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
source_group("" FILES ${XI_TOOLS_SOURCE_FILES})
foreach(xiTool ${XI_TOOLS_SOURCE_FILES})
  get_filename_component(xiToolName ${xiTool} NAME_WE)
  set(xiToolTarget "Xi.Tools.${xiToolName}")
  add_executable(${xiToolTarget} ${xiTool})
  target_link_libraries(
      ${xiToolTarget}

      PRIVATE
          Common
          Logging
          Crypto
          P2P
          Rpc
          Serialization
          System
          Crypto
          rocksdb
          lz4
          boost

          ${XI_CANONICAL_LIBRARIES}
      )
endforeach() # xiTool

