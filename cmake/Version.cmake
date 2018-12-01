## This section helps us tag our builds with the git commit information
set(versionScript "${CMAKE_CURRENT_LIST_DIR}/VersionScript.cmake")
set(XI_VERSION_INPUT_FILE "${PROJECT_SOURCE_DIR}/VERSION")
set(XI_VERSION_INCLUDE_DIR "${CMAKE_BINARY_DIR}/version")
set(XI_VERSION_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/src/config/version.h.in")
set(XI_VERSION_FILE "${XI_VERSION_INCLUDE_DIR}/version.h")
file(MAKE_DIRECTORY ${XI_VERSION_INCLUDE_DIR})
add_custom_target(
    Version
    COMMAND ${CMAKE_COMMAND}
        -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
        -DXI_VERSION_INPUT_FILE=${XI_VERSION_INPUT_FILE}
        -DXI_VERSION_TEMPLATE=${XI_VERSION_TEMPLATE}
        -DXI_VERSION_INCLUDE_DIR=${XI_VERSION_INCLUDE_DIR}
        -P ${versionScript}
    BYPRODUCT ${XI_VERSION_FILE}
    COMMENT "Updating version information..."
    SOURCES ${XI_VERSION_TEMPLATE} ${XI_VERSION_INPUT_FILE} ${versionScript}
)
