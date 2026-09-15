include(fetcher)

set( MCPL_REPO "https://github.com/mctools/mcpl.git" CACHE STRING "Fallback location (URL or local path) of MCPL sources.")
set( MCPL_MINIMUM_VERSION 2.2.0 CACHE STRING "Minimum version of MCPL (can use git ref-spec).")
option(MCPL_REQUIRE_PREINSTALL "Require pre-installed MCPL (>= MCPL_MINIMUM_VERSION)" OFF)

#Set variables for MCPL config like this, to keep them from poluting the current
#scope:
set(mcpl_fetch_params "")
list( APPEND mcpl_fetch_params MCPL_NOTOUCH_CMAKE_BUILD_TYPE ON)
list( APPEND mcpl_fetch_params MCPL_MODIFY_RPATH ON)#Might not need this depending on environment
list( APPEND mcpl_fetch_params MCPL_BUILD_WITHZLIB ON )#"Whether to link with zlib if
#available." (fixme: should we try to fetchcontent zlib as well?)  The variables
#below are (as per MCPL v1.5.1) all at default values, just included here for
#robustness:
list( APPEND mcpl_fetch_params MCPL_ENABLE_EXAMPLES OFF )
list( APPEND mcpl_fetch_params MCPL_ENABLE_SSW ON )
list( APPEND mcpl_fetch_params MCPL_ENABLE_PHITS ON )
list( APPEND mcpl_fetch_params MCPL_ENABLE_GEANT4 OFF )
list( APPEND mcpl_fetch_params MCPL_ENABLE_FATBINARIES OFF )
list( APPEND mcpl_fetch_params MCPL_ENABLE_PYTHON ON )

if (WINDOWS)
  set( tmp_instprefix "" )
else()
  set( tmp_instprefix "${FLAVOR}/${MCCODE_VERSION}/" )
endif()

list(APPEND mcpl_fetch_params CMAKE_INSTALL_BINDIR "${tmp_instprefix}bin" )
list(APPEND mcpl_fetch_params CMAKE_INSTALL_DATADIR "${tmp_instprefix}data" )#probably won't be used
list(APPEND mcpl_fetch_params MCPL_PYPATH "${tmp_instprefix}python" )#fixme: add to McStas environment PYTHONPATH?

if (WINDOWS)
  list(APPEND mcpl_fetch_params CMAKE_INSTALL_LIBDIR lib )
  list(APPEND mcpl_fetch_params CMAKE_INSTALL_INCLUDEDIR lib)#??
else()
  list(APPEND mcpl_fetch_params CMAKE_INSTALL_LIBDIR "${tmp_instprefix}libs/mcpl" )
  list(APPEND mcpl_fetch_params CMAKE_INSTALL_INCLUDEDIR "${tmp_instprefix}/libs/mcpl" )#??
endif()

unset( tmp_instprefix )

git_fetch(mcpl "${MCPL_MINIMUM_VERSION}" "master" "${MCPL_REPO}" ${MCPL_REQUIRE_PREINSTALL} "${mcpl_fetch_params}")

