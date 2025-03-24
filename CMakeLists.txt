cmake_minimum_required(VERSION 3.17.0)
cmake_policy(VERSION 3.17.0)

if ( "x${MCVERSION}" STREQUAL "x" )
    set(MCVERSION 3.99.999)
    message(WARNING "Build version was unset. Define using e.g. -DMCVERSION=${MCVERSION}")
endif()

# This file will build McStas and/or McXtrace
project(mccode LANGUAGES C VERSION "${MCVERSION}")
set(CMAKE_C_STANDARD 99)
# Stash the project version for use in C macro comparisons
math(EXPR MCCODE_VERSION_MACRO "(${CMAKE_PROJECT_VERSION_MAJOR} * 100 + ${CMAKE_PROJECT_VERSION_MINOR}) * 1000 + ${CMAKE_PROJECT_VERSION_PATCH}" OUTPUT_FORMAT DECIMAL)

# Set module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

#Ensure various variables are set to sensible default values for the target
#platform (if not already set by a toolchain file):

option( BUILD_MCSTAS "Build McStas" OFF )
option( BUILD_MCXTRACE "Build McXtrace" OFF )
option( BUILD_TOOLS "Include tools" ON )
option( BUILD_MCXRUN "Include mcrun/mxrun even if BUILD_TOOLS is OFF" )#fixme: replace with more fine-grained control
option( ENABLE_COMPONENTS "Whether or not to include comps and instruments" ON )
option( EXCLUDE_CONTRIB "Whether or not to exclude contrib comps" OFF )#fixme: replace with more fine-grained control
option( EXCLUDE_DATA "Whether or not to exclude data files" OFF )#fixme: replace with more fine-grained control

if ( BUILD_MCSTAS AND BUILD_MCXTRACE )
    message(WARNING "Configuration requests building both McStas and McXtrace which is likely to fail.")
elseif( NOT BUILD_MCSTAS AND NOT BUILD_MCXTRACE )
    message(FATAL_ERROR "Configuration requires building one of McStas or McXtrace via -DBUILD_MCXTRACE=ON or -DBUILD_MCSTAS=ON")
endif()

# Setup McCode values (from mkdist or defaults)
include(MCUtil)

if ( BUILD_MCSTAS )
  option( ENSURE_NCRYSTAL "Build Third Party code NCrystal if not already available" OFF )
  if ( ENSURE_NCRYSTAL )
    set ( NCRYSTAL_LEGACY_USE 1 )
  else()
    set ( NCRYSTAL_LEGACY_USE 0 )
  endif()
endif()


if( BUILD_MCSTAS )
    setupMCCODE("mcstas")
    message(STATUS "Configuring McStas build ${FLAVOR}")
    add_subdirectory(mcstas)
    if ( ENABLE_COMPONENTS )
      add_subdirectory(mcstas-comps)
    endif()
    add_subdirectory(docpkg/manuals/mcstas)
endif()

if ( BUILD_MCXTRACE )
    setupMCCODE("mcxtrace")
    message(STATUS "Configuring McXtrace build ${FLAVOR}")
    add_subdirectory(mcxtrace)
    if ( ENABLE_COMPONENTS )
      add_subdirectory(mcxtrace-comps)
    endif()
    add_subdirectory(docpkg/manuals/mcxtrace)
endif()

if ( BUILD_TOOLS )
  add_subdirectory( tools )
elseif ( BUILD_MCXRUN )
  add_subdirectory( tools/Python/mcrun )
  add_subdirectory( tools/Python/mccodelib)
endif()

# Find/Fetch dependencies:

option( ENSURE_MCPL "Build Third Party code MCPL if not already available" OFF )
if( ENSURE_MCPL )
  include( MCPL )
endif()

if( BUILD_MCSTAS )
  if ( ENSURE_NCRYSTAL )
    include( NCrystal )
  endif()
endif()

