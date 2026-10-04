cmake_minimum_required( VERSION 3.19 )

# This will install McStas manuals
set(FLAVOR mcstas)
project(${FLAVOR} C)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/Modules/")

message( "CMAKE_SOURCE_DIR=        ${CMAKE_SOURCE_DIR}" )
message( "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}" )
message( "CMAKE_CURRENT_LIST_DIR=  ${CMAKE_CURRENT_LIST_DIR}" )
message( "PROJECT_SOURCE_DIR=      ${PROJECT_SOURCE_DIR}" )
message( "PROJECT_BINARY_DIR=      ${PROJECT_BINARY_DIR}" )

message( "include MCUtil" )
include(MCUtil)
message( "setup MCCODE" )
setupMCCODE("${FLAVOR}")

# CPack configuration
message( "CPack configuration" )
set(CPACK_PACKAGE_NAME          "${FLAVOR}-manuals")
set(CPACK_RESOURCE_FilE_LICENSE "${PROJECT_SOURCE_DIR}/../COPYING")
set(CPACK_PACKAGE_CONTACT       "pkwi@fysik.dtu.dk")

set(CPACK_PACKAGE_VERSION       "${MCCODE_MAJOR}.${MCCODE_MINOR}.${MCCODE_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MCCODE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MCCODE_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MCCODE_PATCH}")

# NSIS
set(NSIS_NAME "Manuals, ${MCCODE_VERSION}")
set(CPACK_NSIS_PACKAGE_NAME "${NSIS_NAME}")
set(CPACK_NSIS_DISPLAY_NAME "${NSIS_NAME}")

set(MANUAL "${FLAVOR}-manual.pdf")
set(CMANUAL "${FLAVOR}-components.pdf")

set(MCCODE_LIB "${DEST_DATADIR_COMPS}")
set(MANDEST "${DEST_DATADIR_DOC}")

# Debian
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${FLAVOR}")
set(CPACK_DEBIAN_PACKAGE_REPLACES "${FLAVOR}-manuals-3.5.1")

include(CPack)

install(FILES "${PROJECT_SOURCE_DIR}/manual.pdf"
  DESTINATION "${MANDEST}"
  RENAME "${MANUAL}"
)

install(FILES "${PROJECT_SOURCE_DIR}/Component_manual.pdf"
  DESTINATION "${MANDEST}"
  RENAME "${CMANUAL}"
)

# --- HTML manuals: ship the .tgz archives alongside the PDFs, and extract
# each into its own subdirectory under ${DEST_DATADIR_DOC} at install time.
install(FILES "${PROJECT_SOURCE_DIR}/manual-html.tgz"
  DESTINATION "${MANDEST}"
)

install(FILES "${PROJECT_SOURCE_DIR}/Component_manual-html.tgz"
  DESTINATION "${MANDEST}"
)

install(CODE "
  set(_extract_dir \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MANDEST}/${FLAVOR}-manual\")
  file(MAKE_DIRECTORY \"\${_extract_dir}\")
  execute_process(
    COMMAND \"${CMAKE_COMMAND}\" -E tar xzf \"${PROJECT_SOURCE_DIR}/manual-html.tgz\"
    WORKING_DIRECTORY \"\${_extract_dir}\"
  )
")

install(CODE "
  set(_extract_dir \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MANDEST}/${FLAVOR}-components\")
  file(MAKE_DIRECTORY \"\${_extract_dir}\")
  execute_process(
    COMMAND \"${CMAKE_COMMAND}\" -E tar xzf \"${PROJECT_SOURCE_DIR}/Component_manual-html.tgz\"
    WORKING_DIRECTORY \"\${_extract_dir}\"
  )
")
