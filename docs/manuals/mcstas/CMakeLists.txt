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
file(GLOB LIST CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_LIST_DIR}" "*.tex" "*.sty" "*.bib" "*.bst" "[a-z]*/*.tex" "[a-z]*/*.parms" "figures/*")

# TeX snippets
foreach(NAME sources samples optics monitors misc union sasmodels contrib obsolete)
   file(GLOB ${NAME}_COMP_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/../../../mcstas-comps/${NAME}/*.comp")
   add_custom_command(
     OUTPUT "${NAME}.done"
     COMMAND "mcdoc" "--tex" "--in-repo" "--dir=${CMAKE_SOURCE_DIR}/../../../mcstas-comps/${NAME}" "--outdir=${CMAKE_CURRENT_SOURCE_DIR}"
     COMMAND "${CMAKE_COMMAND}" "-E" "touch" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.done"
     DEPENDS "${CMAKE_SOURCE_DIR}/../../../tools/Python/mcdoc/mcdoc.py" ${${NAME}_COMP_SOURCES}
     WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
     COMMENT "Regenerating ${NAME} LaTeX component/instrument doc snippets (in-place in the source tree)"
     )
endforeach()

# MD files
foreach(NAME sources samples optics monitors misc union sasmodels contrib obsolete)
   file(GLOB ${NAME}_COMP_SOURCES_MD CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/../../../mcstas-comps/${NAME}/*.comp")
   add_custom_command(
     OUTPUT "${NAME}.mddone"
     COMMAND "mcdoc" "--md" "--in-repo" "--dir=${CMAKE_SOURCE_DIR}/../../../mcstas-comps/${NAME}"
     COMMAND "${CMAKE_COMMAND}" "-E" "touch" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.mddone"
     DEPENDS "${CMAKE_SOURCE_DIR}/../../../tools/Python/mcdoc/mcdoc.py" ${${NAME}_COMP_SOURCES_MD}
     WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
     COMMENT "Regenerating ${NAME} Markdown component/instrument doc snippets"
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
  DEPENDS sources.done samples.done optics.done monitors.done misc.done union.done sasmodels.done contrib.done obsolete.done
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

# --- HTML manual generation (tex4ht, MathJax math rendering), packaged as .tgz ---
# Multi-pass build: htlatex -> bibtex -> makeindex -> htlatex x2 (to settle
# cross-references and citations across the split HTML pages), then tar/gzip
# the resulting page set. Requires texlive-plain-generic (htlatex/tex4ht) and
# dvipng; skipped with a warning if htlatex isn't available rather than
# failing the whole manuals build.
find_program(HTLATEX_EXECUTABLE htlatex)
find_program(BASH_EXECUTABLE bash)
if(HTLATEX_EXECUTABLE AND BASH_EXECUTABLE)
  foreach(DOC manual Component_manual)
    add_custom_command(
      OUTPUT "${PROJECT_BINARY_DIR}/${DOC}-html.tgz"
      COMMAND "${BASH_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/generate-xbb-files.sh"
      COMMAND "${HTLATEX_EXECUTABLE}" "${DOC}.tex" "html,3,mathjax" " -cvalidate" ""
      COMMAND "bibtex" "${DOC}"
      COMMAND "makeindex" "${DOC}.idx"
      COMMAND "${HTLATEX_EXECUTABLE}" "${DOC}.tex" "html,3,mathjax" " -cvalidate" ""
      COMMAND "${HTLATEX_EXECUTABLE}" "${DOC}.tex" "html,3,mathjax" " -cvalidate" ""
      COMMAND "${BASH_EXECUTABLE}" "-c" "cat '${CMAKE_CURRENT_SOURCE_DIR}/html-style-overrides.css' >> '${DOC}.css'"
      COMMAND "${BASH_EXECUTABLE}" "-c" "tar czf '${DOC}-html.tgz' ${DOC}*.html ${DOC}.css figures 2>/dev/null || tar czf '${DOC}-html.tgz' ${DOC}*.html ${DOC}.css"
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
      DEPENDS "${PROJECT_BINARY_DIR}/${DOC}.pdf" "${CMAKE_CURRENT_SOURCE_DIR}/html-style-overrides.css" "${CMAKE_CURRENT_SOURCE_DIR}/generate-xbb-files.sh"
      COMMENT "Building HTML version of ${DOC} (tex4ht/MathJax) and packaging as ${DOC}-html.tgz"
      VERBATIM
      )
    add_custom_target("${DOC}_html" ALL DEPENDS "${PROJECT_BINARY_DIR}/${DOC}-html.tgz")
  endforeach()

  install(FILES "${PROJECT_BINARY_DIR}/manual-html.tgz"
    DESTINATION "${MANDEST}"
    RENAME "${FLAVOR}-manual-html.tgz"
    OPTIONAL
  )
  install(FILES "${PROJECT_BINARY_DIR}/Component_manual-html.tgz"
    DESTINATION "${MANDEST}"
    RENAME "${FLAVOR}-components-html.tgz"
    OPTIONAL
  )
else()
  message(WARNING "htlatex and/or bash not found -- skipping HTML manual generation. Install texlive-plain-generic (htlatex/tex4ht) and dvipng to enable the .tgz HTML manuals.")
endif()
