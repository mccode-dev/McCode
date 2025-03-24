cmake_minimum_required(VERSION 3.17.0)
cmake_policy(VERSION 3.17.0)

# This will install McStas components
project(python-tools-mccodelib C)

message(STATUS "Configure ${CMAKE_PROJECT_NAME} ${FLAVOR} ${PROJECT_NAME}")

# Choose between McStas or McXtrace
option(BUILD_MCSTAS   "Choose the McStas flavor" off)
option(BUILD_MCXTRACE "Choose the McXtrace flavor (trumps BUILD_MCSTAS)" Off)


#Option normally needed only for the conda pkg recipe:
option(MCCODE_BUILD_CONDA_PKG   "If current build will be relocatable conda package" OFF)

# Set module path
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# Set McCode values (from mkdist or defaults)
include(MCUtil)

if (${BUILD_MCXTRACE})
  set(FLAVOR "mcxtrace")
elseif(${BUILD_MCSTAS})
  set(FLAVOR "mcstas")
else()
  message(FATAL_ERROR "No flavor specified!\n"
    "Please use either -DBUILD_MCSTAS=1 or -DBUILD_MCXTRACE=1")
endif()

setupMCCODE("${FLAVOR}")

# Select prefix
if(${BUILD_MCSTAS})
  set(P "mc")
else()
  set(P "mx")
endif()

set(TOOLS_NAME "Python/mccodelib")

set(TOOLS_LIB "${DEST_TOOLDIR}/${TOOLS_NAME}")
set(MCCODE_LOCATION "${DEST_TOOLDIR}")


# Set some extra options for mccode_config
set(CC "${CMAKE_C_COMPILER}")
if (APPLE)
  set(CFLAGS "-headerpad_max_install_names ${CMAKE_C_FLAGS} ${MCCODE_CFLAGS}")
else()
  set(CFLAGS "${CMAKE_C_FLAGS} ${MCCODE_CFLAGS}")
endif()

# CPack configuration
set(CPACK_PACKAGE_NAME          "${FLAVOR}-tools-python-mccodelib")
set(CPACK_RESOURCE_FilE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_CONTACT       "jsbn@fysik.dtu.dk")

set(CPACK_PACKAGE_VERSION       "${MCCODE_MAJOR}.${MCCODE_MINOR}.${MCCODE_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MCCODE_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${MCCODE_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${MCCODE_PATCH}")


# NSIS
set(NSIS_NAME "Python internal library, ${MCCODE_STRING}")
set(CPACK_NSIS_PACKAGE_NAME "${NSIS_NAME}")
set(CPACK_NSIS_DISPLAY_NAME "${NSIS_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${NSIS_NAME}")

# Debian
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${FLAVOR}, python3, python3-ply, hdf-compass")
set(CPACK_DEBIAN_PACKAGE_REPLACES "${FLAVOR}-tools-python-mccodelib-3.5.1")

# RPM
if (RPMPROFILE)
  if (RPMPROFILE STREQUAL "fedora")
    set(PYDEPS "python3, python3-ply")
  else()
    set(PYDEPS ${FLAVOR}-miniconda3-${MCCODE_VERSION})
  endif()
else()
  set(PYDEPS ${FLAVOR}-miniconda3-${MCCODE_VERSION})
endif()

set(CPACK_RPM_PACKAGE_REQUIRES
  "${FLAVOR}-${MCCODE_VERSION}, ${PYDEPS}"
)

include(CPack)

set(WORK "${PROJECT_BINARY_DIR}/work")

set(entries_with_conda_prefix "CFLAGS;NEXUSFLAGS;MPIFLAGS;MCPLFLAGS;NCRYSTALFLAGS;GSLFLAGS;XRLFLAGS;OACCFLAGS;TOOLS_CC;MPIRUN;MPICC")
foreach( var ${entries_with_conda_prefix} )
  set( MCCODE_FINAL_${var} "${${var}}" )
endforeach()

if ( MCCODE_BUILD_CONDA_PKG )
  #Conda pkgs mode need special treatment, to make sure compiler name is not an
  #absolute path into the build conda env:
  set( MCCODE_IS_CONDA_PKG "1" )
  set( conda_prefix "$ENV{CONDA_PREFIX}" )
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set( isysroot "$ENV{CONDA_BUILD_SYSROOT}" )
  endif()
  foreach( var ${entries_with_conda_prefix} )
    message("Investigating variable ${var}")
    if(NOT "${conda_prefix}" STREQUAL "")
      string(REPLACE "${conda_prefix}" "\$\{CONDA_PREFIX\}" MCCODE_FINAL_${var} "${MCCODE_FINAL_${var}}")
      if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	message(STATUS "Change:\n --> -isysroot ${isysroot} -> -isysroot \$\{XCRUN_DETECTED\} if present in MCCODE_FINAL_${var}")
	string(REPLACE "${isysroot}" "\$\{XCRUN_DETECTED\}" MCCODE_FINAL_${var} "${MCCODE_FINAL_${var}}")
      endif()
    endif()
    message(STATUS "-> MCCODE_FINAL_${var}=${MCCODE_FINAL_${var}}")
  endforeach()
  #get_filename_component(MCCODE_FINAL_TOOLS_CC "${MCCODE_FINAL_TOOLS_CC}" NAME)
else()
  set( MCCODE_IS_CONDA_PKG "0" )
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mccode_config.py.in" "${WORK}/mccode_config.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mccode_config.json.in" "${WORK}/mccode_config.json" @ONLY)

# Python files
foreach(name __init__.py fcparticleparser.py utils.py flowchart.py instrgeom.py instrparser.py mcdisplayutils.py mcplotloader.py nodetree.py particleparser.py pipetools.py plotgraph.py pqtgfrontend.py cflags.py)
  install(
    FILES "${PROJECT_SOURCE_DIR}/${name}"
    DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
    )
endforeach()

install(
  FILES "${WORK}/mccode_config.py" "${WORK}/mccode_config.json"
  DESTINATION "${DEST_TOOLDIR}/${TOOLS_NAME}"
  )
