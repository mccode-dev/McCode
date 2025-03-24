# InstallMCCODE
# A module for configuring and installing McStas / McXtrace
# The following macros needs to be defined before calling this:
# NAME, FLAVOR, FLAVOR_FMT, FLAVOR_LIB,
# MCCODE_PARTICLE, MCCODE_LIBENV, MCCODE_PROJECT
# MAJOR, MINOR, MCCODE_VERSION, MCCODE_NAME, MCCODE_DATE,
# MCCODE_STRING MCCODE_TARNAME
#
# After doing so (using set()) this module can be included with

macro(AppendDef def)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND
      PROPERTY COMPILE_DEFINITIONS
      ${def}
      )
endmacro(AppendDef)

macro(AppendDefIf def)
  if(${def})
    AppendDef(${def}=${def})
  endif()
endmacro(AppendDefIf)


macro(installMCCODE)

  # Ignore CMake warning when setting "-DBUILD_MCSTAS=1"
  option(enable_${FLAVOR} "This option is here for compatibility only." On)
  if (NOT enable_${FLAVOR})
    message(FATAL_ERROR "Cannot deselect ${FLAVOR} flavor.")
  endif()

  ## CPack configuration
  set(CPACK_PACKAGE_NAME          "${FLAVOR}")
  set(CPACK_RESOURCE_FilE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../COPYING")
  set(CPACK_PACKAGE_CONTACT       "pkwi@fysik.dtu.dk")

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
  ## Debian
  set(CPACK_DEBIAN_PACKAGE_DEPENDS       "build-essential, libopenmpi-dev, bash")
  set(CPACK_DEBIAN_PACKAGE_RECOMMENDS    "${FLAVOR}-comps")
  set(CPACK_DEBIAN_PACKAGE_SUGGESTS      "")
  set(CPACK_DEBIAN_PACKAGE_REPLACES "${FLAVOR}-3.5.1")

  ## FreeBSD
  set(CPACK_FREEBSD_PACKAGE_LICENSE "GPLv3")

  ## NSIS
  set(CPACK_NSIS_PACKAGE_NAME "${MCCODE_STRING}")
  set(CPACK_NSIS_DISPLAY_NAME "${MCCODE_STRING}")
  set(CPACK_NSIS_UNINSTALL_NAME "${FLAVOR}-uninstall")
  
  include(CPack)
  
  string(TIMESTAMP MCCODE_YEAR "%Y")

  ## Add global definitions
  # set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_DEFINITIONS
  AppendDef(MCCODE_NAME="${MCCODE_NAME}")
  AppendDef(MCCODE_TARNAME="${MCCODE_TARNAME}")
  AppendDef(MCCODE_VERSION="${MCCODE_VERSION}")
  AppendDef(MCCODE_STRING="${MCCODE_STRING}")
  AppendDef(MCCODE_BUGREPORT="www.${MCCODE_TARNAME}.org")
  AppendDef(MCCODE_URL="")

	# -DCC_HAS_PROTOS=1
	# -DSTDC_HEADERS=1
	# -DHAVE_THREADS=\"-DUSE_THREADS\ \$$OPENMP_CFLAGS\ \"



  ## User-adjustable options
  option (USE_NEXUS
    "Support the NEXUS file format" OFF)

  option (USE_THREADS
    "Enable threading; OBSOLETE: Use MPI/SSH grid feature instead." OFF)

  # update definitions to match choices
  if (USE_NEXUS)
    AppendDef(HAVE_NEXUS="-DUSE_NEXUS -lNeXus")
    AppendDef(USE_NEXUS)
  endif()

  AppendDefIf(USE_THREADS)


  ## Functionality needed to check dependencies
  include (CheckFunctionExists)
  include (CheckLibraryExists)
  include (CheckIncludeFiles)


  # A macro for ensuring that all of the values in a variable list are true
  macro(check_vars vars errmsg)
    foreach(var ${vars})
      if(NOT ${var})
        # throw fatal error when seeing a false value
        message(FATAL_ERROR ${errmsg})
      endif()
    endforeach()
  endmacro()


  ## Check system configuration and dependencies

  # REQUIRED
  check_function_exists(malloc      HAVE_MALLOC)
  check_function_exists(realloc     HAVE_REALLOC)
  check_vars(
    "${HAVE_MALLOC};${HAVE_REALLOC}"
    "Missing either malloc or realloc!")

  check_include_files("stdlib.h"    HAVE_STDLIB_H)
  check_include_files("memory.h"    HAVE_MEMORY_H)
  check_include_files("unistd.h"    HAVE_UNISTD_H)
  check_vars(
    "${HAVE_STDLIB_H};${HAVE_MEMORY_H};${HAVE_UNISTD_H}"
    "Missing either stdlib.h, memory.h or unistd.h!"
    )

  check_include_files("inttypes.h"  HAVE_INT_TYPES_H)
  check_include_files("stdint.h"    HAVE_STD_INT_H)
  check_vars(
    "${HAVE_INT_TYPES_H};${HAVE_STD_INT_H}"
    "Missing either inttypes.h or stdint.h"
    )

  check_include_files("sys/types.h" HAVE_SYS_TYPES_H)
  check_include_files("sys/stat.h"  HAVE_SYS_STAT_H)
  check_vars(
    "${HAVE_SYS_TYPES_H};${HAVE_SYS_STAT_H}"
    "Missing either sys/types.h or sys/stat.h"
    )

  check_include_files("string.h"    HAVE_STRING_H)
  check_include_files("strings.h"   HAVE_STRINGS_H)
  check_vars(
    "${HAVE_STRING_H};${HAVE_STRINGS_H}"
    "Missing either string.h or strings.h"
    )


  # Check for math
  check_include_files("math.h" HAVE_MATH_H)
  if(NOT HAVE_MATH_H)
    message(FATAL_ERROR "Error: Cannot find math.h [m]")
  endif()


  # Check for BISON and FLEX (will fail if they are not found)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")
  message("-- Looking for bison and flex")
  find_package(FLEX)
  if ( NOT FLEX_FOUND )
    if ( WIN32 )
      find_program( FLEX_EXECUTABLE "win_flex.exe" )
      if ( FLEX_EXECUTABLE )
        set( FLEX_FOUND "ON" )
      endif()
    endif()
    if ( NOT FLEX_FOUND )
      message( FATAL_ERROR "Could not locate flex executable." )
    endif()
  endif()
  find_package(BISON)
  if ( NOT BISON_FOUND )
    if ( WIN32 )
      find_program( BISON_EXECUTABLE "win_bison.exe" )
      if ( BISON_EXECUTABLE )
        set( BISON_FOUND "ON" )
      endif()
    endif()
    if ( NOT BISON_FOUND )
      message( FATAL_ERROR "Could not locate bison executable." )
    endif()
  endif()

  message("-- Looking for bison and flex - found")


  # OPTIONAL
  check_function_exists(strcasecmp  HAVE_STRCASECMP)
  check_function_exists(strcasestr  HAVE_STRCASESTR)
  check_function_exists(fdopen      HAVE_FDOPEN)
  check_function_exists(qsort       HAVE_QSORT)

  # Update definitions
  AppendDefIf(HAVE_STRCASECMP)
  AppendDefIf(HAVE_STRCASESTR)
  AppendDefIf(HAVE_FDOPEN)
  AppendDefIf(HAVE_QSORT)



  # Create work directory, where all rewritten source files go to
  # (to support in-place builds)
  file(MAKE_DIRECTORY "work")
  file(MAKE_DIRECTORY "work/support")


  ## Rules for processing files while expanding macros

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

  configure_directory ("lib/COPYING" "work")
  configure_directory ("lib/share/*" "work/lib/share")

  configure_directory ("src/*" "work/src")

  ## Include source directories for building
  include_directories(
    "${PROJECT_BINARY_DIR}/work/src"             # rewritten files from output dir
    "${PROJECT_BINARY_DIR}/work/lib/share"       # rewritten library files
    "${PROJECT_SOURCE_DIR}/${FLAVOR_LIB}/share"  # lib depending on flavor (nlib/xlib)
  )


  ## Generate lex.yy.c with flex
  add_custom_command(
    OUTPUT work/src/lex.yy.c
    COMMAND "${FLEX_EXECUTABLE}" -i "${PROJECT_SOURCE_DIR}/src/instrument.l"
    WORKING_DIRECTORY work/src
    DEPENDS "${PROJECT_SOURCE_DIR}/src/instrument.l"
  )
  ## Generate py-lex.yy.c with flex
  add_custom_command(
    OUTPUT work/src/py-lex.yy.c
    COMMAND "${FLEX_EXECUTABLE}" -o py-lex.yy.c -i "${PROJECT_SOURCE_DIR}/src/py-instrument.l"
    WORKING_DIRECTORY work/src
    DEPENDS "${PROJECT_SOURCE_DIR}/src/py-instrument.l"
  )
  ## Generate instrument.tab.{h,c} with bison
  add_custom_command(
    OUTPUT work/src/instrument.tab.h work/src/instrument.tab.c
    COMMAND "${BISON_EXECUTABLE}" -v -d "${PROJECT_SOURCE_DIR}/src/instrument.y"
    WORKING_DIRECTORY work/src
    DEPENDS "${PROJECT_SOURCE_DIR}/src/instrument.y" work/src/lex.yy.c
  )
  ## Generate py-instrument.tab.{h,c} with bison
  add_custom_command(
    OUTPUT work/src/py-instrument.tab.h work/src/py-instrument.tab.c
    COMMAND "${BISON_EXECUTABLE}" -v -d "${PROJECT_SOURCE_DIR}/src/py-instrument.y"
    WORKING_DIRECTORY work/src
    DEPENDS "${PROJECT_SOURCE_DIR}/src/py-instrument.y" work/src/py-lex.yy.c
  )


  # Handling of system-provided random functions on windows - 
  # needed only in the link step for mccode and -format
  if(WINDOWS)
    AppendDef(random=rand)
    AppendDef(srandom=srand)
  endif()

  ## Build executable for flavor
  add_executable(
    ${FLAVOR}
    work/src/cexp.c
    work/src/cogen.c
    work/src/coords.c
    work/src/debug.c
    work/src/file.c
    work/src/list.c
    work/src/mccode.h
    work/src/memory.c
    work/src/port.c
    work/src/port.h
    work/src/symtab.c
    work/src/re.c
    work/src/metadata.c

    # files generated with flex and bison
    work/src/lex.yy.c
    work/src/instrument.tab.h
    work/src/instrument.tab.c
  )

  add_executable(
    ${FLAVOR}-pygen
    work/src/py-cexp.c
    work/src/pygen.c
    work/src/coords.c
    work/src/debug.c
    work/src/file.c
    work/src/list.c
    work/src/memory.c
    work/src/port.c
    work/src/port.h
    work/src/symtab.c
    work/src/re.c
    work/src/metadata.c

    # files generated with flex and bison
    work/src/py-lex.yy.c
    work/src/py-instrument.tab.h
    work/src/py-instrument.tab.c
  )

  if ( CMAKE_INSTALL_PREFIX AND NOT "x${CMAKE_INSTALL_PREFIX}" STREQUAL "x/" )
    get_filename_component( tmp "${CMAKE_INSTALL_PREFIX}/${DEST_RESOURCEDIR}" REALPATH )
    target_compile_definitions(${FLAVOR}-pygen PRIVATE "MCCODE_RESOURCEDIR_HARDCODED=${tmp}")
    target_compile_definitions(${FLAVOR} PRIVATE "MCCODE_RESOURCEDIR_HARDCODED=${tmp}")
  endif()

  ## Add install targets
  include(MCUtil)
  set(WORK "${PROJECT_BINARY_DIR}/work")

  #license file:
  install( FILES "${WORK}/COPYING" DESTINATION "${DEST_DATADIR_INFO}")
  #General library:
  file( GLOB general_codefiles "${WORK}/lib/share/*.h"  "${WORK}/lib/share/*.c" )
  install( FILES ${general_codefiles} DESTINATION "${DEST_DATADIR_CODEFILES}")

  # Flavor-specific library
  file( GLOB flavor_codefiles "${FLAVOR_LIB}/share/*.h" "${FLAVOR_LIB}/share/*.c" "${FLAVOR_LIB}/share/*.cl" )
  install( FILES ${flavor_codefiles} DESTINATION "${DEST_DATADIR_CODEFILES}" )

  if(NOT WINDOWS)
    # Binaries
    install (
      TARGETS ${FLAVOR}
      DESTINATION "${DEST_BINDIR}"
    )

    install (
      TARGETS ${FLAVOR}-pygen
      DESTINATION "${DEST_BINDIR}"
    )

    configure_file(
      cmake/support/run-scripts/environment.in
      work/support/${FLAVOR}-environment
      @ONLY)
    install(PROGRAMS ${WORK}/support/${FLAVOR}-environment DESTINATION "${DEST_DATADIR_TOPENVFILES}")

    configure_file(
      cmake/support/run-scripts/labenv.in
      work/support/${FLAVOR}-labenv
      @ONLY)
    install(PROGRAMS ${WORK}/support/${FLAVOR}-labenv DESTINATION "${DEST_DATADIR_TOPENVFILES}")

    configure_file(
      cmake/support/run-scripts/module.in
      work/support/${FLAVOR}-module
      @ONLY)
    install(PROGRAMS ${WORK}/support/${FLAVOR}-module DESTINATION "${DEST_DATADIR_TOPENVFILES}")

    install(PROGRAMS ${WORK}/support/${FLAVOR}-postinst DESTINATION "${DEST_BINDIR}")
    install(PROGRAMS ${WORK}/support/${FLAVOR}_errmsg DESTINATION "${DEST_BINDIR}")
  endif()

  if(WINDOWS)
    # Generate and install Windows setup scripts
    foreach (name env.bat env.m go.bat test.bat)
      configure_file(
        cmake/support/run-scripts/mccode${name}.in
        work/support/${FLAVOR}${name}
        )
      install(PROGRAMS ${WORK}/support/${FLAVOR}${name} DESTINATION "${DEST_BINDIR}")
    endforeach()

    configure_file(
      cmake/support/run-scripts/labenv.bat.in
      work/support/${FLAVOR}-labenv.bat
      @ONLY)
    install(PROGRAMS ${WORK}/support/${FLAVOR}-labenv.bat DESTINATION "${DEST_BINDIR}")

    # Python/Perl related batches special handling
    foreach (name run.bat doc.bat test.bat viewtest.bat resplot.bat plot.bat display.bat gui.bat guistart.bat plot-pyqtgraph.bat plot-matplotlib.bat plot-matlab.bat display-webgl.bat display-webgl-classic.bat display-pyqtgraph.bat display-cad.bat display-matplotlib.bat display-mantid.bat)
      configure_file(
	      cmake/support/run-scripts/${name}.in
	      work/support/${MCCODE_PREFIX}${name}
	      )
      install(PROGRAMS ${WORK}/support/${MCCODE_PREFIX}${name} DESTINATION "${DEST_BINDIR}")
    endforeach()

    # Binaries
    install (
      TARGETS ${FLAVOR}
      DESTINATION "${DEST_BINDIR}"
    )

    # Binaries
    install (
      TARGETS ${FLAVOR}-pygen
      DESTINATION "${DEST_BINDIR}"
    )
    if ( MCCODE_USE_LEGACY_DESTINATIONS )
      install(PROGRAMS
        cmake/support/run-scripts/mpirun.bat
        DESTINATION "${DEST_BINDIR}"
      )

      configure_file(
        cmake/support/run-scripts/mpicc.bat.in
        work/support/mpicc.bat
      )

      install(PROGRAMS
        ${CMAKE_CURRENT_SOURCE_DIR}/work/support/mpicc.bat
        DESTINATION "${DEST_BINDIR}"
      )
    endif()

  endif()


endmacro(installMCCODE)
