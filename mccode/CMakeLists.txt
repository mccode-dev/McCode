cmake_minimum_required( VERSION 3.19 )

# This file will build McStas
project(mcstas C)
set(CMAKE_C_STANDARD 99)

# Set module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# Setup McCode values (from mkdist or defaults)
include(MCUtil)
if ( BUILD_MCSTAS )
  setupMCCODE("mcstas")
elseif ( BUILD_MCXTRACE )
  setupMCCODE("mcxtrace")
endif()

# Setup install target
include(InstallMCCODE)
installMCCODE()

# Configure docs
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mccode.md.in" "${WORK}/${FLAVOR}.md" @ONLY)

install(
  FILES "${WORK}/${FLAVOR}.md"
  DESTINATION "${DEST_DATADIR_DOC}"
)
