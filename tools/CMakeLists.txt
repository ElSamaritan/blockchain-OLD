file(GLOB XI_TOOLS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
source_group("" FILES ${XI_TOOLS_SOURCE_FILES})
foreach(xiTool ${XI_TOOLS_SOURCE_FILES})
  get_filename_component(xiToolName ${xiTool} NAME_WE)
  set(xiToolTarget "Xi.Tools.${xiToolName}")
  add_executable(${xiToolTarget} ${xiTool})
endforeach() # xiTool

target_link_libraries(Xi.Tools.GenerateHashes PRIVATE Common Crypto CryptoNoteCore)
