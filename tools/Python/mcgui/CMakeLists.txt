cmake_minimum_required(VERSION 3.17.0)
cmake_policy(VERSION 3.17.0)

# This will install McStas or McXtrace mc/mxgui
project(python-tools-mcgui C)

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

set(TOOLS_NAME "Python/${P}gui/")

# CPack configuration
set(CPACK_PACKAGE_NAME          "${FLAVOR}-tools-python-${P}gui")
set(CPACK_RESOURCE_FilE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_CONTACT       "pkwi@fysik.dtu.dk")

set(CPACK_PACKAGE_VERSION       "${MCCODE_MAJOR}.${MCCODE_MINOR}.${MCCODE_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MCCODE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MCCODE_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MCCODE_PATCH}")


# NSIS
set(NSIS_NAME "Python-based mcgui (mcgui-py), ${MCCODE_STRING}")
set(CPACK_NSIS_PACKAGE_NAME "${NSIS_NAME}")
set(CPACK_NSIS_DISPLAY_NAME "${NSIS_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${NSIS_NAME}")

# Debian
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${FLAVOR}, ${FLAVOR}-tools-python-${P}doc, ${FLAVOR}-tools-python-mccodelib, python3, python3-pyqt6.qsci")
set(CPACK_DEBIAN_PACKAGE_REPLACES "${FLAVOR}-tools-python-${P}gui-3.5.1")

# RPM
if (RPMPROFILE)
   if (RPMPROFILE STREQUAL "fedora")
        set(PYDEPS "python3, python3-qt5, python3-qscintilla-qt5")
   else()
	set(PYDEPS "")
   endif()
else()
	set(PYDEPS "")
endif()

set(CPACK_RPM_PACKAGE_REQUIRES
  "${FLAVOR}-${MCCODE_VERSION}, ${FLAVOR}-tools-python-${P}doc-${MCCODE_VERSION}, ${FLAVOR}-tools-python-mccodelib-${MCCODE_VERSION}, qt5-qtbase-gui")

include(CPack)

set(WORK "${PROJECT_BINARY_DIR}/work")

# Configure fallback script
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mcgui.in" "${WORK}/${P}gui" @ONLY)

# Configure launcher
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/McCode-py.desktop.in" "${WORK}/${FLAVOR}-${MCCODE_VERSION}-py.desktop")

# Main script, including rename mcgui/mxgui
install(
  PROGRAMS "${PROJECT_SOURCE_DIR}/mcgui.py"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  RENAME "${P}gui.py"
  )

# Other .py infrastructure
foreach(NAME viewclasses.py run-icon.png compile-icon.png widgets.py ${FLAVOR}-py.png)
  install(
    FILES "${PROJECT_SOURCE_DIR}/${NAME}"
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
    )
endforeach()

# Wrapper-scripts on windows are handled via
# cmake Modules/MCUtil
if(NOT WINDOWS)
  add_custom_target(
    "CREATE_${PROJECT_NAME}_SYMLINK" ALL DEPENDS "${WORK}/${P}gui"
    )

  install(
    FILES "${WORK}/${P}gui"
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
    PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )
  install(
    PROGRAMS "${WORK}/${P}gui"
    DESTINATION ${DEST_BINDIR}
    )
    
  # Install desktop entry
  install(FILES "${WORK}/${FLAVOR}-${MCCODE_VERSION}-py.desktop" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications" )

  # Install icon
  install(FILES "${FLAVOR}-py.png" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps/")
endif()
