include_guard()

include(PlatformDefaults)

# Install library files into lib/${FLAVOR}, while skipping unneeded files
macro(installLib path)
  set(dest "${DEST_TOOLDIR}")

  install (
    DIRECTORY "${path}"
    DESTINATION "${dest}"
    PATTERN "Makefile*" EXCLUDE  # skip makefiles
    PATTERN "#*"        EXCLUDE  # skip backup files
    PATTERN ".*"        EXCLUDE  # skip hidden files
    PATTERN "*.out"     EXCLUDE  # skip binary files
    PATTERN "*.*.in"    EXCLUDE  # skip configure processed template files
  )
endmacro()

  # Macro for configuring every file in a directory
  # *.in files are configured, while other files are copied unless target exists
  macro(configure_directory IN_GLOB OUT_DIR)
    file(GLOB tmp "${IN_GLOB}")
    foreach(file_in ${tmp})
      get_filename_component(filename "${file_in}" NAME)      # /doc/man/example.1.in -> example.1.in
      string(REGEX MATCH "^(.+)\\.in" matches "${filename}")  # example.1.in -> example.1
      if(matches)
        # from IN/doc/man/example.1.in -> OUT/doc/man/example.1
        configure_file (
          "${file_in}"
          "${OUT_DIR}/${CMAKE_MATCH_1}"
          )
      else()
        # do not overwrite files created by configure
        if(NOT (EXISTS "${OUT_DIR}/${filename}") OR ("${file_in}" IS_NEWER_THAN "${OUT_DIR}/${filename}"))
          if( IS_SYMLINK "${file_in}" )
            #follow symlink
            get_filename_component( file_in "${file_in}" REALPATH )
          endif()
          file( COPY "${file_in}" DESTINATION "${OUT_DIR}")
        endif()
      endif()
    endforeach()
  endmacro()

# Check whether we are being run through mkdist
macro(isMkDist outvar)
  string(CONFIGURE "@MCCODE_NAME@" TMP @ONLY) # TMP is empty unless MCCODE_NAME is set already
  string(LENGTH "${TMP}" ${outvar})
endmacro()


# Setup McCode constants using either mkdist or Git-defaults
macro(setupMCCODE FLAVOR)

  # Use .pl suffix on any platform
  set(PERL_SUFFIX "pl")

  # Check for WINDOWS
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WINDOWS true)
  endif()

  # Set Nexus linking flag
  if(USE_NEXUS)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS}  -lNeXus")
    set(CMAKE_C_LFLAGS "${CMAKE_C_LFLAGS} -lNeXus")
  endif()

  # Set 32-bit flags
  if(ARCH EQUAL "i386")
    set(CMAKE_C_FLAGS  "-m32")
    set(CMAKE_C_LFLAGS "${CMAKE_C_LFLAGS} -m32")
  endif()

  if(DEFINED ARCH)
    message(STATUS "Compiling for ${ARCH} ${CMAKE_SYSTEM_NAME}")
  else()
    message(STATUS "Compiling for ${CMAKE_SYSTEM_NAME}")
  endif()


  # Set macros
  if("${FLAVOR}" STREQUAL "mcstas")
    set(FLAVOR           "mcstas")
    set(FLAVOR_UPPER     "MCSTAS")

    set(FLAVOR_LIB       "nlib")
    set(MCCODE_LIBENV    "${FLAVOR_UPPER}")

    set(MCCODE_PARTICLE  "neutron")
    set(MCCODE_PARTICLE_CODE 2112)
    set(MCCODE_PROJECT    1)

    set(MCCODE_PREFIX     "mc")
  endif()

  if("${FLAVOR}" STREQUAL "mcxtrace")
    set(FLAVOR           "mcxtrace")
    set(FLAVOR_UPPER     "MCXTRACE")

    set(FLAVOR_LIB       "xlib")
    set(MCCODE_LIBENV    "${FLAVOR_UPPER}")

    set(MCCODE_PARTICLE "xray")
    set(MCCODE_PROJECT   2)
    set(MCCODE_PARTICLE_CODE 22)

    set(MCCODE_PREFIX     "mx")
  endif()

  # Set fallback "year"
  if("${MCCODE_YEAR}" STREQUAL "")
    set(MCCODE_YEAR "2100")
  endif()

  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY COMPILE_DEFINITIONS
    FLAVOR="${FLAVOR}" FLAVOR_UPPER="${FLAVOR_UPPER}"
    FLAVOR_LIB="${FLAVOR_LIB}"
    MCCODE_LIBENV="${MCCODE_LIBENV}" MCCODE_PARTICLE="${MCCODE_PARTICLE}"
    MCCODE_PROJECT=${MCCODE_PROJECT}
    )

  # Check for mkdist values
  isMkDist(MKDIST)

  if(MKDIST)
    ## Set mkdist-provided version
    string(CONFIGURE "@MCCODE_VERSION@"   MCCODE_VERSION @ONLY)
    string(CONFIGURE "@MCCODE_NAME@"      MCCODE_NAME @ONLY)    # e.g. McStas or McXtrace
    string(CONFIGURE "@MCCODE_DATE@"      MCCODE_DATE @ONLY)    # e.g. MONTH. DAY, YEAR
    string(CONFIGURE "@MCCODE_STRING@"    MCCODE_STRING @ONLY)  # MCCODE_NAME MCCODE_VERSION - MCCODE_DATE
    string(CONFIGURE "@MCCODE_TARNAME@"   MCCODE_TARNAME @ONLY)
    string(CONFIGURE "@MCCODE_PARTICLE@"  MCCODE_PARTICLE @ONLY)  # neutron or xray
    string(CONFIGURE "@FLAVOR_UPPER@"     FLAVOR_UPPER @ONLY)   # MCSTAS or MCXTRACE
    string(CONFIGURE "@FLAVOR@"           FLAVOR @ONLY)         # mcstas or mcxtrace
    string(CONFIGURE "@MCCODE_PREFIX@"    MCCODE_PREFIX @ONLY)  # mc or mx
    
    message(STATUS "Using provided settings MCCODE_VERSION=${MCCODE_VERSION}, MCCODE_NAME=${MCCODE_NAME}, MCCODE_DATE=${MCCODE_DATE}, MCCODE_STRING=${MCCODE_STRING}, MCCODE_TARNAME=${MCCODE_TARNAME}")
  else()
    ## Set Git-specific version
    if (MCVERSION)
      set(MCCODE_VERSION "${MCVERSION}")
    else ()
      set(MCCODE_VERSION "3.999.999-git")
    endif()
    set(MCCODE_NAME "${FLAVOR}")
    set(MCCODE_DATE "git")
    set(MCCODE_STRING "${NAME} ${MCCODE_VERSION}, ${MCCODE_DATE}")
    set(MCCODE_TARNAME "${FLAVOR}")
  endif()

  # Split incoming MCCODE_VERSION to list
  string(REPLACE "." ";" MCCODE_VERSION_LIST ${MCCODE_VERSION})
  list(LENGTH MCCODE_VERSION_LIST MCCODE_VERSION_LIST_LEN)

  ## CPack Package versioning
  set(MCCODE_MAJOR 3)
  set(MCCODE_MINOR 999)
  set(MCCODE_PATCH 999)
  if (${MCCODE_VERSION_LIST_LEN} GREATER 0)
    list(GET MCCODE_VERSION_LIST 0 MCCODE_MAJOR)
  endif()
  if (${MCCODE_VERSION_LIST_LEN} GREATER 1)
    list(GET MCCODE_VERSION_LIST 1 MCCODE_MINOR)
  endif()
  if (${MCCODE_VERSION_LIST_LEN} GREATER 2)
    list(GET MCCODE_VERSION_LIST 2 MCCODE_PATCH)
  endif()

  set(CPACK_PACKAGE_VERSION       "${MCCODE_MAJOR}.${MCCODE_MINOR}.${MCCODE_PATCH}")
  set(CPACK_PACKAGE_VERSION_MAJOR "${MCCODE_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${MCCODE_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${MCCODE_PATCH}")

  include(Locations)
  # During migration set these as well:
  set( bin "${DEST_BINDIR}" )
  set( lib "${DEST_LIBDIR}" )

  # Set default installation paths
  foreach(name bin doc etc include lib man sbin share src)
    if(NOT(DEFINED ${name}))
      set(${name} "${name}")
    endif()
  endforeach()

  if(WINDOWS)
    # Fix installation root
    if ( MCCODE_USE_LEGACY_DESTINATIONS )
      set(CMAKE_INSTALL_PREFIX "${FLAVOR}-${MCCODE_VERSION}")
      set(CPACK_NSIS_INSTALL_ROOT "C:\\\\${FLAVOR}-${MCCODE_VERSION}")
    else()
      set(CPACK_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
      set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
    endif()

    set(CPACK_NSIS_UNINSTALL_NAME "${CMAKE_PROJECT_NAME}-uninstall")

    # Set BIN and LIB paths
    set(MCCODE_BIN "${DEST_BINDIR}")
    set(MCCODE_LIB "${DEST_LIBDIR}")
    # Replace '/' with '\'
    string(REPLACE "/" "\\\\" MCCODE_BIN "${MCCODE_BIN}")
    string(REPLACE "/" "\\\\" MCCODE_LIB "${CMAKE_INSTALL_PREFIX}/${DEST_DATADIR_COMPS}")
  else()
    set(MCCODE_BIN "${DEST_BINDIR}")
    set(MCCODE_LIB "${CMAKE_INSTALL_PREFIX}/${DEST_DATADIR_COMPS}")
  endif()

  #FIXME: ^^ Perhaps we should use something other than MCCODE_LIB for this path

  # Helper for adding leading "."
  macro(addDot name val)
    if(NOT DEFINED ${name} AND NOT ${val} STREQUAL "")
      set(${name} ".${val}")
    endif()
  endmacro()

  # Set instrument suffix (after compilation)
  if(NOT DEFINED MCCODE_EXECUTABLE_SUFFIX)
    if(DEFINED EXE_SUFFIX)
      set(MCCODE_EXECUTABLE_SUFFIX "${EXE_SUFFIX}")
    else()
      set(MCCODE_EXECUTABLE_SUFFIX "out")
    endif()
  endif()

  # Define suffix-macros that include a leading dot "."
  addDot(DOT_OUT_SUFFIX "${OUT_SUFFIX}")

  addDot(DOT_PYTHON_SUFFIX "${PYTHON_SUFFIX}")
  addDot(DOT_PERL_SUFFIX   "${PERL_SUFFIX}")

  if(WINDOWS)
    # On windows we actually do -pl.bat in case of Perl
    set(PL_TOOL_SUFFIX "-pl.bat")
  else()
    set(PL_TOOL_SUFFIX ".pl")
  endif()


  # Add some special Windows/Unix CPACK configuration
  if(WINDOWS)
    if ( MCCODE_USE_LEGACY_DESTINATIONS )
      # Fix installation folder (installs to ${ROOT}\${DIRECTORY})
      set(CPACK_PACKAGE_INSTALL_DIRECTORY "")

      # Windows program files do not have any version-suffix
      set(PROGRAM_SUFFIX "")

      # Create desktop links for mcgui py/pl and mccodego batch files
      set(CPACK_NSIS_CREATE_ICONS "CreateShortCut '$DESKTOP\\\\${MCCODE_PREFIX}gui-${MCCODE_VERSION}.lnk' '${CPACK_NSIS_INSTALL_ROOT}\\\\${DEST_BINDIR}\\\\${MCCODE_PREFIX}guistart.bat' ")
     set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$DESKTOP\\\\${FLAVOR}-shell-${MCCODE_VERSION}.lnk' '${CPACK_NSIS_INSTALL_ROOT}\\\\${DEST_BINDIR}\\\\${FLAVOR}go.bat' ")
    endif()
  else()
  
  # Have CMake respect install prefix
  message(STATUS "Install prefix -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
  set(CPACK_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
  set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

  set(CPACK_RPM_PACKAGE_RELOCATABLE TRUE)

  # Avoid e.g. /usr/local being "part" of the RPMs
  set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST
    ${CMAKE_INSTALL_PREFIX}
    ${CMAKE_INSTALL_PREFIX}/${FLAVOR}
    ${CMAKE_INSTALL_PREFIX}/${FLAVOR}/${MCCODE_VERSION}
    ${DEST_BINDIR}
    ${DEST_TOOLDIR}
    ${DEST_DATADIR_LIBDIR}
    ${DEST_DATADIR_CODEFILES}
    )

  # Add "-VERSION" to all program files (executables)
  set(PROGRAM_SUFFIX "-${MCCODE_VERSION}")

  # Run postinst and postrm scripts for various platforms
  set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "work/support/postinst;work/support/postrm")
  set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${PROJECT_BINARY_DIR}/work/support/postinst;")
  set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${PROJECT_BINARY_DIR}/work/support/postrm;")
    
  # Define dependencies for gcc and the like
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "build-essential, libopenmpi-dev")
  set(CPACK_RPM_PACKAGE_REQUIRES "gcc, openmpi-devel")

  # Specify license for FreeBSD pkg
  set(CPACK_FREEBSD_PACKAGE_LICENSE "GPLv3")

  # Generate postinst and postrm scripts
  configure_file(
    cmake/support/install-scripts/postinst.in
    work/support/${FLAVOR}-postinst
    @ONLY)
  configure_file(
    cmake/support/install-scripts/postrm.in
    work/support/${FLAVOR}-postrm
    @ONLY)
  configure_file(
    cmake/support/install-scripts/postinst.in
    work/support/postinst
    @ONLY)
  configure_file(
    cmake/support/install-scripts/postrm.in
    work/support/postrm
    @ONLY)

  # Generate the console-errormessage wrapper
  configure_file(
    cmake/support/run-scripts/mccode_errmsg.in
    "work/support/${FLAVOR}_errmsg"
    @ONLY)

  # Set architecture
  if(ARCH EQUAL "amd64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE    "x86_64")
  elseif(ARCH EQUAL "i386")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE    "i686")
  else()
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${ARCH}")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE    "${ARCH}")
  endif()

  endif()
endmacro()

# Helper function which can look for input files. Apparently "file(GLOB ...)" is
# frowned upon by some people. However, the only provided alternative (hardcode
# all your filenames) is rather unappealing. We glob for files, but apply the
# CONFIGURE_DEPENDS keyword.

function( file_globsrc output_var )
  set(res "")
  foreach( pattern ${ARGN} )
    if ( NOT IS_ABSOLUTE "${pattern}")
      set(pattern "${PROJECT_SOURCE_DIR}/${pattern}")
    endif()
    file(GLOB tmp LIST_DIRECTORIES OFF CONFIGURE_DEPENDS "${pattern}" )
    #TODO: actually test the below
    foreach( f ${tmp} )
      if ( "${f}" MATCHES "~" )
        continue()
      endif()
      if ( "${f}" MATCHES "\\.in$" )
        continue()
      endif()
      if ( "${f}" MATCHES "#" )
        continue()
      endif()
      if ( "${f}" MATCHES "^\\." )
        continue()
      endif()
      #    PATTERN "Makefile*" EXCLUDE  # skip makefiles
      #    PATTERN "*.out"     EXCLUDE  # skip binary files
      list( APPEND res "${f}" )
    endforeach()
  endforeach()
  set(${output_var} "${res}" PARENT_SCOPE)
endfunction()
