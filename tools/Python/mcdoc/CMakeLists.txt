cmake_minimum_required(VERSION 3.17.0)
cmake_policy(VERSION 3.17.0)

# This will install McStas or McXtrace mc/mxdoc
project(python-tools-mcdoc C)

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

set(TOOLS_NAME "Python/${P}doc/")

# CPack configuration
set(CPACK_PACKAGE_NAME          "${FLAVOR}-tools-python-${P}doc")
set(CPACK_RESOURCE_FilE_LICENSE "${PROJECT_SOURCE_DIR}/../../COPYING")
set(CPACK_PACKAGE_CONTACT       "pkwi@fysik.dtu.dk")

set(CPACK_PACKAGE_VERSION       "${MCCODE_MAJOR}.${MCCODE_MINOR}.${MCCODE_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MCCODE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MCCODE_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MCCODE_PATCH}")


# NSIS
set(NSIS_NAME "Python-based mcdoc, ${MCCODE_STRING}")
set(CPACK_NSIS_PACKAGE_NAME "${NSIS_NAME}")
set(CPACK_NSIS_DISPLAY_NAME "${NSIS_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${NSIS_NAME}")

# Debian
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${FLAVOR}, ${FLAVOR}-comps")

# mcdoc now installs the flat tool-doc pages (mcrun.md, mcgui.md, mctest.md,
# mcplot.md, mcdisplay.md, ...) that used to be shipped by those tools' own
# packages (see each one's now-removed <tool>.md.in configure/install
# step). Any version of those packages older than this one still owns the
# same paths under DEST_DATADIR_DOC, so dpkg refuses the overwrite unless
# we declare it here: Replaces lets this package take the files, Breaks
# forces dpkg/apt to upgrade or remove the older sibling package in
# lock-step instead of leaving it installed with a file mcdoc has since
# taken over (which is what caused "trying to overwrite .../mcgui.md").
set(CPACK_DEBIAN_PACKAGE_REPLACES
  "${FLAVOR}-tools-python-${P}doc-3.5.1, ${FLAVOR}-tools-python-${P}run (<< ${MCCODE_VERSION}), ${FLAVOR}-tools-python-${P}gui (<< ${MCCODE_VERSION}), ${FLAVOR}-tools-python-${P}test (<< ${MCCODE_VERSION}), ${FLAVOR}-tools-python-${P}plot-pyqtgraph (<< ${MCCODE_VERSION}), ${FLAVOR}-tools-python-${P}display-pyqtgraph (<< ${MCCODE_VERSION})"
)
set(CPACK_DEBIAN_PACKAGE_BREAKS
  "${FLAVOR}-tools-python-${P}run (<< 3.8.0), ${FLAVOR}-tools-python-${P}gui (<< 3.8.0), ${FLAVOR}-tools-python-${P}test (<< 3.8.0), ${FLAVOR}-tools-python-${P}plot-pyqtgraph (<< 3.8.0), ${FLAVOR}-tools-python-${P}display-pyqtgraph (<< 3.8.0)"
)

# RPM
if (RPMPROFILE)
   if (RPMPROFILE STREQUAL "fedora")
        set(PYDEPS "python3")
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
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mcdoc.in" "${WORK}/${P}doc" @ONLY)

# Main script, including rename mcdoc/mxdoc
install(
  FILES "${PROJECT_SOURCE_DIR}/mcdoc.py"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  RENAME "${P}doc.py"
  PERMISSIONS
  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# Wrapper-scripts on windows are handled via
# cmake Modules/MCUtil
if(NOT WINDOWS)
  add_custom_target(
    "CREATE_${PROJECT_NAME}_SYMLINK" ALL DEPENDS "${WORK}/${P}doc"
    )

  install(
    FILES "${WORK}/${P}doc"
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
    PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )
  install(
    PROGRAMS "${WORK}/${P}doc"
    DESTINATION ${DEST_BINDIR}
  )
endif()

  # Flat, flavor-agnostic tool documentation for the whole Python/ tool tree.
  # These hand-maintained .md/.html pages replace the old per-tool
  # <tool>.md.in -> <tool>.md configure/install steps (now removed from
  # each tool's own CMakeLists.txt); mcdoc installs the full set once,
  # to the same DEST_DATADIR_DOC destination those used to go to.

foreach(TOOLDOCS_PAGE ${TOOLDOCS_PAGES})
  get_filename_component(TOOLDOCS_PAGE_REAL "${PROJECT_SOURCE_DIR}/${TOOLDOCS_PAGE}.md" REALPATH)
  install(
    FILES "${TOOLDOCS_PAGE_REAL}"
    DESTINATION "${DEST_DATADIR_DOC}"
  )
endforeach()

# HTML pages use the same basenames, except mcdoc itself: the source tree
# keeps that one as mcdoc_tool.html, since a plain mcdoc.html would clash
# with the mcdoc/mxdoc master overview page that `mcdoc --install`
# (re)generates at this same DEST_DATADIR_DOC destination.
set(TOOLDOCS_HTML_PAGES README codegen mcrun mcgui mcplot mcdisplay mctest mcdoc_tool)

foreach(TOOLDOCS_HTML_PAGE ${TOOLDOCS_HTML_PAGES})
  get_filename_component(TOOLDOCS_HTML_PAGE_REAL "${PROJECT_SOURCE_DIR}/${TOOLDOCS_HTML_PAGE}.html" REALPATH)
  install(
    FILES "${TOOLDOCS_HTML_PAGE_REAL}"
    DESTINATION "${DEST_DATADIR_DOC}"
  )
endforeach()

# McStas/McXtrace language grammar reference (kept generic, not
# per-flavor). Installed under its own Component/ and Instrument/
# subdirectories so the relative cross-links between the two pages
# keep working once installed. Same REALPATH treatment as above.
get_filename_component(GRAMMAR_COMPONENT_MD_REAL   "${PROJECT_SOURCE_DIR}/Component/COMPONENT_Grammar.md"   REALPATH)
get_filename_component(GRAMMAR_COMPONENT_HTML_REAL  "${PROJECT_SOURCE_DIR}/Component/COMPONENT_Grammar.html" REALPATH)
get_filename_component(GRAMMAR_INSTRUMENT_MD_REAL   "${PROJECT_SOURCE_DIR}/Instrument/INSTRUMENT_Grammar.md"   REALPATH)
get_filename_component(GRAMMAR_INSTRUMENT_HTML_REAL "${PROJECT_SOURCE_DIR}/Instrument/INSTRUMENT_Grammar.html" REALPATH)

install(
  FILES "${GRAMMAR_COMPONENT_MD_REAL}"
        "${GRAMMAR_COMPONENT_HTML_REAL}"
  DESTINATION "${DEST_DATADIR_DOC}/Component"
)
install(
  FILES "${GRAMMAR_INSTRUMENT_MD_REAL}"
        "${GRAMMAR_INSTRUMENT_HTML_REAL}"
  DESTINATION "${DEST_DATADIR_DOC}/Instrument"
)


