# Fetches the contributed mcstas-chopper-lib repo (Greg Tucker,
# https://github.com/mcdotstar/mcstas-chopper-lib) at configure time and
# installs its pieces into the three McStas resource locations they belong
# in. This is deliberately *not* built as a CMake subproject: chopper-lib's
# own CMakeLists.txt builds a compiled chopper_lib library plus its CTest
# suite (and requires CMake >= 3.25 to do so), but McStas has no use for a
# compiled library here -- chopper-lib.c/.h are %include-d as raw source
# directly into generated instrument C, exactly like the existing
# mcstas-comps/share/*-lib.c snippets. We therefore only Populate the
# source tree (content_fetch, see fetcher.cmake) and copy/install specific
# files out of it; the 3.25 floor in chopper-lib's own CMakeLists.txt is
# never evaluated.
include(fetcher)

set( CHOPPERLIB_REPO "https://github.com/mcdotstar/mcstas-chopper-lib.git" CACHE STRING
     "Location (URL or local path) of mcstas-chopper-lib sources." )
set( CHOPPERLIB_VERSION "v4.1.0" CACHE STRING
     "Git tag/ref of mcstas-chopper-lib to fetch. Pinned rather than tracking a branch: \
chopper-lib is young, under active development, and its own README documents breaking \
field renames between major versions." )

content_fetch(chopperlib "${CHOPPERLIB_VERSION}" "${CHOPPERLIB_REPO}")

# 1) C library snippets, %include-d by components/instruments -> resources/share
#    (alongside e.g. monitor_nd-lib.c)
install( FILES "${chopperlib_SOURCE_DIR}/chopper-lib.c"
               "${chopperlib_SOURCE_DIR}/chopper-lib.h"
         DESTINATION "${DEST_DATADIR_CODEFILES}" )

# 2) Components -> resources/contrib
file( GLOB CHOPPERLIB_COMPS "${chopperlib_SOURCE_DIR}/*.comp" )
install( FILES ${CHOPPERLIB_COMPS} DESTINATION "${DEST_DATADIR_COMPS}/contrib" )

# 3) Instruments -> one folder per instrument under resources/examples/Tests_optics,
#    matching the Test_Xxx/Test_Xxx.instr convention already used there.
file( GLOB CHOPPERLIB_INSTRUMENTS "${chopperlib_SOURCE_DIR}/*.instr" )
foreach( chopperlib_instr ${CHOPPERLIB_INSTRUMENTS} )
  get_filename_component( chopperlib_instr_name "${chopperlib_instr}" NAME_WE )
  install( FILES "${chopperlib_instr}"
           DESTINATION "${DEST_DATADIR_EXAMPLES}/Tests_optics/${chopperlib_instr_name}" )
endforeach()
unset( chopperlib_instr )
unset( chopperlib_instr_name )

message( STATUS "mcstas-chopper-lib ${CHOPPERLIB_VERSION}: staged from ${chopperlib_SOURCE_DIR}" )
