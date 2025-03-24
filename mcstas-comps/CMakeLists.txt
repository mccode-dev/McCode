cmake_minimum_required( VERSION 3.17.0 )

# This will install McStas components
project( mcstas-comps C )

# Set module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Set McCode values (from mkdist or defaults)
include(MCUtil)
setupMCCODE("mcstas")

option( ENABLE_CIF2HKL "Build Third Party code cif2hkl (fortran)" ON )#TODO: Only enable enable Fortran if this is on
if ( ENABLE_CIF2HKL )
  set(DEBIDEPS "${FLAVOR}, libnexus-dev, libgsl-dev")
else()
  set(DEBIDEPS "${FLAVOR}, libnexus-dev, libgsl-dev, cif2hkl")
endif()

set(WORK "${PROJECT_BINARY_DIR}/work")

# CPack configuration
set(CPACK_PACKAGE_NAME          "${FLAVOR}-comps")
set(CPACK_RESOURCE_FilE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_CONTACT       "pkwi@fysik.dtu.dk")

set(CPACK_PACKAGE_VERSION       "${MCCODE_MAJOR}.${MCCODE_MINOR}.${MCCODE_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MCCODE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MCCODE_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MCCODE_PATCH}")


set(CPACK_DEBIAN_PACKAGE_DEPENDS "${DEBIDEPS}")
set(CPACK_DEBIAN_PACKAGE_REPLACES "${FLAVOR}-comps-3.5.1")


# NSIS
set(CPACK_NSIS_PACKAGE_NAME "${MCCODE_STRING} Components")
set(CPACK_NSIS_DISPLAY_NAME "${MCCODE_STRING} Components")

include(CPack)

if ( ENABLE_CIF2HKL )
  enable_language( Fortran )
  add_executable(
    cif2hkl
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cif2hkl/cif2hkl.F90
    )
  target_compile_options(cif2hkl PUBLIC "-ffree-line-length-512")
  install (
    TARGETS cif2hkl
    DESTINATION "${DEST_BINDIR}"
    )
endif()

option( ENABLE_NEUTRONICS "Build Third Party code neutronics (fortran)" ON )#TODO: Only enable enable Fortran if this is on
if ( ENABLE_NEUTRONICS )
  enable_language( Fortran )
  add_library(neutronics "STATIC" libs/neutronics/neutronics-subs.f)
  install(TARGETS neutronics DESTINATION "${DEST_LIBDIR}/neutronics")
  #NB (Aug 2023): Removed general "libs" which has the side-effect of not
  #installing other files related to "neutronics".
endif()

# System c-code
install( DIRECTORY "share/" DESTINATION "${DEST_DATADIR_CODEFILES}")

# Editor syntax-highlighting etc
if (NOT WINDOWS)
  file(GLOB EDITORS editors/*) # follow links (install DIRECTORY does not)
  install( FILES ${EDITORS} DESTINATION "${DEST_DATADIR_EDITORS}" )
endif()

# System comps
install( DIRECTORY "misc/"      DESTINATION "${DEST_DATADIR_COMPS}/misc")
install( DIRECTORY "monitors/"  DESTINATION "${DEST_DATADIR_COMPS}/monitors")
install( DIRECTORY "obsolete/"  DESTINATION "${DEST_DATADIR_COMPS}/obsolete")
install( DIRECTORY "optics/"    DESTINATION "${DEST_DATADIR_COMPS}/optics")
install( DIRECTORY "samples/"   DESTINATION "${DEST_DATADIR_COMPS}/samples")
install( DIRECTORY "sources/"   DESTINATION "${DEST_DATADIR_COMPS}/sources")
install( DIRECTORY "union/"     DESTINATION "${DEST_DATADIR_COMPS}/union")
install( DIRECTORY "sasmodels/" DESTINATION "${DEST_DATADIR_COMPS}/sasmodels")

# Contrib comps
if ( NOT EXCLUDE_CONTRIB )
  install( DIRECTORY "contrib/" DESTINATION "${DEST_DATADIR_COMPS}/contrib")
endif()

# Data files
if ( NOT EXCLUDE_DATA )
  install( DIRECTORY "data/"    DESTINATION "${DEST_DATADIR_COMPS}/data")
endif()

install( DIRECTORY "examples/"  DESTINATION "${DEST_DATADIR_EXAMPLES}")


# Include mcstas-comp revision tag file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/revision.in" "${WORK}/revision" @ONLY)

# copy some descriptory files
configure_directory ("NOMENCLATURE.md" "work/nomenclature")
configure_directory ("README.md" "work/readme")
install( FILES "${WORK}/nomenclature/NOMENCLATURE.md" DESTINATION "${DEST_DATADIR_COMPS}")
install( FILES "${WORK}/readme/README.md" DESTINATION "${DEST_DATADIR_COMPS}")
install( FILES "${WORK}/revision" DESTINATION "${DEST_DATADIR_COMPS}" )
