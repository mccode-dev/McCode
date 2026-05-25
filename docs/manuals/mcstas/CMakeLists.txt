cmake_minimum_required( VERSION 3.17.0 )

# This will install McStas components
set(FLAVOR mcstas)
project(${FLAVOR}_manuals C)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/Modules/")

message( "CMAKE_SOURCE_DIR=        ${CMAKE_SOURCE_DIR}" )
message( "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}" )
message( "CMAKE_CURRENT_LIST_DIR=  ${CMAKE_CURRENT_LIST_DIR}" )
message( "PROJECT_SOURCE_DIR=      ${PROJECT_SOURCE_DIR}" )
message( "PROJECT_BINARY_DIR=      ${PROJECT_BINARY_DIR}" )

message( "include MCUtil" )
include(MCUtil)
message( "include UseLATEX" )
include(UseLATEX)
# set(PDFLATEX_COMPILER "/usr/bin/xelatex")
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

if(WINDOWS)
  set(MCCODE_LIB "${CMAKE_INSTALL_PREFIX}/")
  set(MANDEST "${lib}/doc/manuals")
else()
  set(MCCODE_LIB "${CMAKE_INSTALL_PREFIX}/${MCCODE_NAME}")
  set(MANDEST "${FLAVOR}/${MCCODE_VERSION}/doc/manuals")
endif()


# Debian
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${FLAVOR}")
set(CPACK_DEBIAN_PACKAGE_REPLACES "${FLAVOR}-manuals-3.5.1")

include(CPack)

message( "LaTeX configuration" )
# set(LATEX_OUTPUT_PATH ".")
file(GLOB LIST RELATIVE "${CMAKE_CURRENT_LIST_DIR}" "*.tex" "*.sty" "*.bib" "*.bst" "[m-s]*/*.tex" "[m-s]*/*.parms" "figures/*")

# TeX snippets
foreach(NAME sources samples optics monitors misc)
   # PWFIXME: ONLY SEMI-FUNCTIONAL
   add_custom_command(
     OUTPUT "${NAME}.done"
     COMMAND "mcdoc" "--tex" "--in-repo" "--dir=${CMAKE_SOURCE_DIR}/../../../mcstas-comps/${NAME}" "--outdir=${PROJECT_BINARY_DIR}"
     WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
     )
endforeach()

# MD files
foreach(NAME sources samples optics monitors misc)
   # PWFIXME: ONLY SENMI-FUNCTIONAL
   add_custom_command(
     OUTPUT "${NAME}.mddone"
     COMMAND "mcdoc" "--md" "--in-repo" "--dir=${CMAKE_SOURCE_DIR}/../../../mcstas-comps/${NAME}"
     WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
     )
endforeach()

set(LATEX_OUTPUT_PATH "${PROJECT_BINARY_DIR}")
add_latex_document(
  manual.tex 
  INPUTS ${LIST}
  DEPENDS sources.done samples.done optics.done monitors.done misc.done
  BIBFILES "mcstas.bib"
  USE_INDEX
  FORCE_PDF
  )
add_latex_document(
  Component_manual.tex 
  INPUTS ${LIST}
  DEPENDS sources.done samples.done optics.done monitors.done misc.done
  BIBFILES "mcstas.bib"
  USE_INDEX
  FORCE_PDF
  )

install(FILES "${PROJECT_BINARY_DIR}/manual.pdf"
  DESTINATION "${MANDEST}"
  RENAME "${MANUAL}"
)

install(FILES "${PROJECT_BINARY_DIR}/Component_manual.pdf"
  DESTINATION "${MANDEST}"
  RENAME "${CMANUAL}"
)
