cmake_minimum_required(VERSION 3.17.0)
cmake_policy(VERSION 3.17.0)

# This will install McStas or McXtrace mc/mxplot-matplotlib
project(python-tools-mcplot-matplotlib C)

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

set(TOOLS_NAME "Python/${P}plot/matplotlib")

# CPack configuration
set(CPACK_PACKAGE_NAME          "${FLAVOR}-tools-python-${P}plot-matplotlib")
set(CPACK_RESOURCE_FilE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_CONTACT       "pkwi@fysik.dtu.dk")

set(CPACK_PACKAGE_VERSION       "${MCCODE_MAJOR}.${MCCODE_MINOR}.${MCCODE_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MCCODE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MCCODE_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MCCODE_PATCH}")


# NSIS
set(NSIS_NAME "Matplotlib-based mcplot (mcplot-matplotlib), ${MCCODE_STRING}")
set(CPACK_NSIS_PACKAGE_NAME "${NSIS_NAME}")
set(CPACK_NSIS_DISPLAY_NAME "${NSIS_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${NSIS_NAME}")

# Debian
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${FLAVOR}, python3-matplotlib, python3-pyqt6, python3-mplexporter, python3-tornado")
set(CPACK_DEBIAN_PACKAGE_REPLACES "${FLAVOR}-tools-python-${P}plot-matplotlib-3.5.1")

# RPM
if (RPMPROFILE)
   if (RPMPROFILE STREQUAL "fedora")
        set(PYDEPS "python3-matplotlib, python3-qt5, python3-tornado")
   else()
	set(PYDEPS "")
   endif()
else()
	set(PYDEPS "")
endif()

set(CPACK_RPM_PACKAGE_REQUIRES
  "${FLAVOR}-${MCCODE_VERSION}, ${PYDEPS}"
)

include(CPack)

set(WORK "${PROJECT_BINARY_DIR}/work")

# Configure fallback script
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mcplot.in" "${WORK}/${P}plot-matplotlib" @ONLY)

# Configure doc
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mcplot-matplotlib.md.in" "${WORK}/${P}plot-matplotlib.md" @ONLY)

# Main script, including rename mc/mxplot
install(
  PROGRAMS "${PROJECT_SOURCE_DIR}/mcplot.py"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  RENAME "${P}plot.py"
  )

# Configure fallback script for the a-b difference plotter
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mcplotdiff.in" "${WORK}/${P}plotdiff-matplotlib" @ONLY)

# Configure doc for the a-b difference plotter
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mcplotdiff-matplotlib.md.in" "${WORK}/${P}plotdiff-matplotlib.md" @ONLY)

# Difference-plotter script, including rename mc/mxplotdiff
install(
  PROGRAMS "${PROJECT_SOURCE_DIR}/mcplotdiff.py"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  RENAME "${P}plotdiff.py"
  )

# Configure fallback script for the a-b co-plotter
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mccoplot.in" "${WORK}/${P}coplot-matplotlib" @ONLY)

# Configure doc for the a-b co-plotter
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mccoplot-matplotlib.md.in" "${WORK}/${P}coplot-matplotlib.md" @ONLY)

# Co-plotter script, including rename mc/mxcoplot
install(
  PROGRAMS "${PROJECT_SOURCE_DIR}/mccoplot.py"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  RENAME "${P}coplot.py"
  )

# Other .py infrastructure
# plotfuncs.py holds the shared, non-CLI plotting/interaction code used by
# both mcplot.py and mcplotdiff.py; it is NOT renamed (unlike mcplot.py),
# so both scripts can import it by a fixed name regardless of flavor.
install(
  FILES "${PROJECT_SOURCE_DIR}/plotfuncs.py"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  )

# Wrapper-scripts on windows are handled via
# cmake Modules/MCUtil
if(NOT WINDOWS)
  add_custom_target(
    "CREATE_${PROJECT_NAME}_SYMLINK" ALL DEPENDS "${WORK}/${P}plot-matplotlib" "${WORK}/${P}plotdiff-matplotlib" "${WORK}/${P}coplot-matplotlib"
    )

  install(
    PROGRAMS ${PROJECT_SOURCE_DIR}/mcplot.py
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
    RENAME "${P}plot.py"
  )

  install(
    PROGRAMS "${WORK}/${P}plot-matplotlib"
    DESTINATION ${DEST_BINDIR}
    )
    
  install(
    FILES "${WORK}/${P}plot-matplotlib.md"
    DESTINATION "${DEST_DATADIR_DOC}"
  )

  install(
    PROGRAMS ${PROJECT_SOURCE_DIR}/mcplotdiff.py
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
    RENAME "${P}plotdiff.py"
  )

  install(
    PROGRAMS "${WORK}/${P}plotdiff-matplotlib"
    DESTINATION ${DEST_BINDIR}
    )

  install(
    FILES "${WORK}/${P}plotdiff-matplotlib.md"
    DESTINATION "${DEST_DATADIR_DOC}"
  )

  install(
    PROGRAMS ${PROJECT_SOURCE_DIR}/mccoplot.py
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
    RENAME "${P}coplot.py"
  )

  install(
    PROGRAMS "${WORK}/${P}coplot-matplotlib"
    DESTINATION ${DEST_BINDIR}
    )

  install(
    FILES "${WORK}/${P}coplot-matplotlib.md"
    DESTINATION "${DEST_DATADIR_DOC}"
  )
endif()
