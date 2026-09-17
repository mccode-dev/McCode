cmake_minimum_required( VERSION 3.17.0 )

# This will install McXtrace manuals
set(FLAVOR mcxtrace)
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
