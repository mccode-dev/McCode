cmake_minimum_required(VERSION 3.19)
cmake_policy(VERSION 3.17.0)

# This will install McStas or McXtrace mc/display-webgl
project(python-tools-mcdisplay-webgl C)

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

set(TOOLS_NAME "Python/${P}display/webgl")

# CPack configuration
set(CPACK_PACKAGE_NAME          "${FLAVOR}-tools-python-${P}display-webgl")
set(CPACK_RESOURCE_FilE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_CONTACT       "pkwi@fysik.dtu.dk")

set(CPACK_PACKAGE_VERSION       "${MCCODE_MAJOR}.${MCCODE_MINOR}.${MCCODE_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MCCODE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MCCODE_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MCCODE_PATCH}")


# NSIS
set(NSIS_NAME "Python/webgl -based ${P}display (${P}display-webgl-py), ${MCCODE_STRING}")
set(CPACK_NSIS_PACKAGE_NAME "${NSIS_NAME}")
set(CPACK_NSIS_DISPLAY_NAME "${NSIS_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${NSIS_NAME}")

# Debian
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${FLAVOR}, python3, python3-ply, python3-numpy")
set(CPACK_DEBIAN_PACKAGE_REPLACES "${FLAVOR}-tools-python-${P}display-webgl-3.5.1")

# RPM
if (RPMPROFILE)
   if (RPMPROFILE STREQUAL "fedora")
        set(PYDEPS "python3, python3-ply, python3-numpy")
   else()
	set(PYDEPS "")
   endif()
else()
	set(PYDEPS "")
endif()

set(CPACK_RPM_PACKAGE_REQUIRES
  "${FLAVOR}-${MCCODE_VERSION},  ${PYDEPS}"
)

include(CPack)

set(WORK "${PROJECT_BINARY_DIR}/work")

# Configure fallback script
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mcdisplay.in" "${WORK}/${P}display" @ONLY)

# Configure index.html template
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/index.html.in" "${WORK}/index.html" @ONLY)

# dist
install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/dist"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
)

# components
install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/components"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  )

# Contexts
install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/Contexts"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
)

# model (imported by several Contexts/ and components/ files)
install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/model"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
)

# data-utils
install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/data-utils"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
)

# utils
install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/utils"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
)

# Main script, including rename mc/mxdisplay
install(
        PROGRAMS "${PROJECT_SOURCE_DIR}/mcdisplay.py"
        DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
        RENAME "${P}display.py"
    )

# Configured index.html
install(
    FILES "${WORK}/index.html"
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
)

# Other .py infrastructure
foreach(NAME package.json vite.config.mts tsconfig.json tsconfig.node.json custom.d.ts mcdisplay.js Lut.js OrbitControls.js dat.gui.min.js jquery.min.js three.min.js template.html __init__.py common.css main.tsx App.tsx)
  install(
    FILES "${PROJECT_SOURCE_DIR}/${NAME}"
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
    )
endforeach()

# Wrapper-scripts on windows are handled via
# cmake Modules/MCUtil
if(NOT WINDOWS)
  install(
    PROGRAMS ${PROJECT_SOURCE_DIR}/mcdisplay.py
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
    RENAME "${P}display.py"
  )

  install(
    PROGRAMS "${WORK}/${P}display"
    DESTINATION ${DEST_BINDIR}
    RENAME "${P}display-webgl"
    )
endif()

# Script for npm instal during install part
if (NOT WINDOWS)
  # Configure fallback script
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/npminstall.in" "${WORK}/npminstall" @ONLY)
  install(
    PROGRAMS "${WORK}/npminstall"
    DESTINATION ${DEST_TOOLDIR}/${TOOLS_NAME}
  )
else()
  # Configure fallback script
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/npminstall.bat.in" "${WORK}/npminstall.bat" @ONLY)
  install(
    PROGRAMS "${WORK}/npminstall.bat"
    DESTINATION ${DEST_TOOLDIR}/${TOOLS_NAME}
  )
endif()
