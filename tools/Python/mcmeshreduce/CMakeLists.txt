cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.17.0)

# This will install McStas or McXtrace mc/mxmeshreduce
project(python-tools-mcmeshreduce C)

message(STATUS "Configure ${CMAKE_PROJECT_NAME} ${FLAVOR} ${PROJECT_NAME}")

# Choose between McStas or McXtrace
option(BUILD_MCSTAS   "Choose the McStas flavor" off)
option(BUILD_MCXTRACE "Choose the McXtrace flavor (trumps BUILD_MCSTAS)" Off)

# Set module path
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# Set McCode values (from mkdist or defaults)
include(MCUtil)

if (${BUILD_MCXTRACE})
  set(FLAVOR "mcxtrace")
  set(CPACK_PACKAGE_HOMEPAGE_URL "http://mcxtrace.org/")
elseif(${BUILD_MCSTAS})
  set(FLAVOR "mcstas")
  set(CPACK_PACKAGE_HOMEPAGE_URL "http://mcstas.org/")
else()
  message(FATAL_ERROR "No flavor specified!\n"
    "Please use either -DBUILD_MCSTAS=1 or -DBUILD_MCXTRACE=1")
endif()

setupMCCODE("${FLAVOR}")

# Select prefix
set(P "${MCCODE_PREFIX}")

set(TOOLS_NAME "Python/${P}meshreduce/")

# CPack configuration
set(CPACK_PACKAGE_NAME          "${FLAVOR}-tools-python-${P}meshreduce")
set(CPACK_RESOURCE_FilE_LICENSE "${PROJECT_SOURCE_DIR}/../../COPYING")
set(CPACK_PACKAGE_CONTACT       "pkwi@fysik.dtu.dk")

set(CPACK_PACKAGE_VERSION       "${MCCODE_MAJOR}.${MCCODE_MINOR}.${MCCODE_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MCCODE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MCCODE_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MCCODE_PATCH}")

# NSIS
set(NSIS_NAME "Python-based ${P}meshreduce, ${MCCODE_STRING}")
set(CPACK_NSIS_PACKAGE_NAME "${NSIS_NAME}")
set(CPACK_NSIS_DISPLAY_NAME "${NSIS_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${NSIS_NAME}")

# Debian: python3-trimesh only from trixie / Ubuntu 25.04, and
# fast-simplification is not packaged; the wrapper reports what is missing
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${FLAVOR}, python3-numpy")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "python3-trimesh")
set(CPACK_RPM_PACKAGE_REQUIRES "${FLAVOR}-${MCCODE_VERSION}")

include(CPack)

set(WORK "${PROJECT_BINARY_DIR}/work")

# Configure fallback script
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mcmeshreduce.in" "${WORK}/${P}meshreduce" @ONLY)

# Main script, including rename mcmeshreduce/mxmeshreduce
install(
  PROGRAMS "${PROJECT_SOURCE_DIR}/mcmeshreduce.py"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  RENAME "${P}meshreduce.py"
  )

# Wrapper-scripts on windows are handled via
# cmake Modules/InstallMCCODE
if(NOT WINDOWS)
  add_custom_target(
    "CREATE_${PROJECT_NAME}_SYMLINK" ALL DEPENDS "${WORK}/${P}meshreduce"
    )
  install(
    PROGRAMS "${WORK}/${P}meshreduce"
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  )
  install(
    PROGRAMS "${WORK}/${P}meshreduce"
    DESTINATION ${DEST_BINDIR}
  )
endif()
