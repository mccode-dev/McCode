# External contributions -- "*.ext" manifests
# ===========================================================================
#
# Replaces the one-off ChopperLib.cmake with a generic mechanism.
#
# An *external contribution* is a set of files (library snippets, components,
# example instruments, data) that live in somebody else's repository but are
# shipped as part of a McCode installation. Rather than hiding that fact in a
# CMake module under cmake/Modules/, each contribution declares itself with a
# small JSON manifest named "<something>.ext", placed **in the directory the
# files are populated into**. A developer looking for, say, a component in
# mcstas-comps/contrib/ therefore finds either the .comp itself or an .ext
# file naming the upstream repository, release and file hash it comes from.
#
# docs/EXTERNAL-CONTRIBUTIONS.md is the prose version of everything below, and
# buildscripts/mcext computes and re-verifies the hashes a manifest records.
#
# ---------------------------------------------------------------------------
# Manifest format
# ---------------------------------------------------------------------------
#
# The short ("flat") form is a JSON array of file entries, each fully
# self-describing:
#
#   [
#     { "name": "chopper-lib.h",
#       "git":  "https://github.com/mcdotstar/mcstas-chopper-lib.git",
#       "url":  "https://raw.githubusercontent.com/.../v4.1.0/chopper-lib.h",
#       "sha256": "0965f666..." },
#     ...
#   ]
#
# The long form is a JSON object holding contribution-wide defaults plus a
# "files" array; every key understood in a file entry may also be given at the
# top level, where it acts as a default for all entries:
#
#   {
#     "name":    "mcstas-chopper-lib",
#     "git":     "https://github.com/mcdotstar/mcstas-chopper-lib.git",
#     "version": "v4.1.0",
#     "license": "BSD-3-Clause",
#     "base":    "https://raw.githubusercontent.com/mcdotstar/mcstas-chopper-lib/v4.1.0/",
#     "archive": { "url": "...tar.gz", "sha256": "...", "strip": 1 },
#     "files": [ { "name": "chopper-lib.h", "sha256": "0965f666..." } ]
#   }
#
# File-entry keys:
#
#   name    (required) file name upstream; also the default install name.
#   sha256  (required) SHA256 of the file contents. The build fails loudly if
#           what arrives does not match, so a moved tag or a rewritten release
#           can never silently change what McCode ships.
#   as      install path relative to the .ext file's own directory. May name
#           a subdirectory ("Foo/Foo.instr"), which is how the one-directory-
#           per-instrument layout under examples/ is reproduced. Default: name.
#   url     explicit download URL for this one file.
#   from    path of the file inside the release archive. Default: name.
#   base    URL prefix; the file is fetched from "<base><from>".
#   git     upstream repository. Informational, but if "base", "url" and
#           "archive" are all absent and "git" points at github.com, a raw
#           base URL is derived from "git" + "version".
#   version upstream tag/ref, used for the derivation above and in messages.
#
# Contribution-wide keys:
#
#   files   (required in the long form) array of file entries.
#   archive { "url", "sha256", "strip" } -- a release tarball/zip. Downloaded
#           and unpacked once; entries without "url"/"base" are copied out of
#           it. "strip" leading path components are dropped (default 1, which
#           matches GitHub's auto-generated source archives).
#   license, description, homepage -- recorded in configure output only.
#
# Resolution order for a single file: "url", else "base", else "archive",
# else derived-from-"git". Mixing is fine: a manifest may take most files from
# a release tarball and one from a direct URL.
#
# ---------------------------------------------------------------------------
# Usage
# ---------------------------------------------------------------------------
#
#   include( External )
#   mccode_install_externals( DIRECTORY "contrib" DESTINATION "${DEST_DATADIR_COMPS}/contrib" )
#
# i.e. one call mirroring each install( DIRECTORY ... ) already present, so
# the staged external files land exactly where the in-tree files of that
# directory do.
#
#   mccode_install_externals(
#       DIRECTORY   <dir>          # absolute, or relative to CMAKE_CURRENT_SOURCE_DIR
#       DESTINATION <dir>          # install destination, as for install( FILES )
#       [ STAGE <dir> ]            # where fetched files are assembled; default
#                                  # ${CMAKE_CURRENT_BINARY_DIR}/externals/<dir>
#       [ COMPONENT <name> ]       # passed through to install()
#       [ NO_RECURSE ]             # only look for *.ext directly in <dir>
#       [ OUTPUT_VARIABLE <var> ]  # staged file paths, set in the caller's scope
#   )
#
# ---------------------------------------------------------------------------
# Cache and offline builds
# ---------------------------------------------------------------------------
#
# Downloads are content-addressed under MCCODE_EXTERNALS_CACHE, so a file
# shared by several manifests is fetched once and survives a wiped build
# directory if the cache is pointed somewhere persistent. Distribution
# packagers who may not fetch during a build have two options: pre-populate
# that cache, or drop the files into MCCODE_EXTERNALS_LOCAL (searched by file
# name, still hash-verified) and set MCCODE_EXTERNALS_OFFLINE=ON so that any
# attempt to reach the network is a hard error rather than a silent download.

include_guard( GLOBAL )

# This module needs string( JSON ) (CMake 3.19) and file( ARCHIVE_EXTRACT )
# (3.18); McCode's cmake_minimum_required was raised to 3.19 to match.

option( ENABLE_EXTERNALS
        "Populate external (*.ext) contributions from their upstream sources" ON )
option( MCCODE_EXTERNALS_OFFLINE
        "Never download: resolve every external file from MCCODE_EXTERNALS_LOCAL or the cache" OFF )
option( MCCODE_EXTERNALS_ALLOW_UNVERIFIED
        "Permit external file entries that carry no sha256 (strongly discouraged)" OFF )
set( MCCODE_EXTERNALS_CACHE "${CMAKE_BINARY_DIR}/externals-cache" CACHE PATH
     "Content-addressed download cache for external (*.ext) contributions." )
set( MCCODE_EXTERNALS_LOCAL "" CACHE PATH
     "Directory of pre-fetched external contribution files, searched by name before downloading." )
set( MCCODE_EXTERNALS_TIMEOUT "60" CACHE STRING
     "Per-file download timeout, in seconds, for external (*.ext) contributions." )

mark_as_advanced( MCCODE_EXTERNALS_ALLOW_UNVERIFIED MCCODE_EXTERNALS_TIMEOUT )


# --- internal helpers ------------------------------------------------------

# Fetch a JSON member, yielding "" rather than an error for absent keys.
function( _mcext_get out_var json )
  string( JSON value ERROR_VARIABLE error GET "${json}" ${ARGN} )
  if ( error )
    set( ${out_var} "" PARENT_SCOPE )
  else()
    set( ${out_var} "${value}" PARENT_SCOPE )
  endif()
endfunction()

# Take the entry value if present, otherwise the contribution-wide default.
function( _mcext_inherit out_var entry defaults key )
  _mcext_get( value "${entry}" ${key} )
  if ( value STREQUAL "" )
    _mcext_get( value "${defaults}" ${key} )
  endif()
  set( ${out_var} "${value}" PARENT_SCOPE )
endfunction()

# Place <url> (hash <sha256>) in the cache and return its path in out_var.
# <label> is a human-readable name used in messages and as the cache key for
# unverified entries.
function( _mcext_cache out_var label url sha256 origin )
  if ( sha256 STREQUAL "" )
    if ( NOT MCCODE_EXTERNALS_ALLOW_UNVERIFIED )
      message( FATAL_ERROR
        "${origin}: entry '${label}' has no \"sha256\". Add the file's SHA256 to the "
        "manifest, or configure with -DMCCODE_EXTERNALS_ALLOW_UNVERIFIED=ON to accept "
        "whatever the server happens to return." )
    endif()
    string( MD5 url_key "${url}" )
    set( cached "${MCCODE_EXTERNALS_CACHE}/url/${url_key}/${label}" )
  else()
    string( TOLOWER "${sha256}" sha256 )
    string( LENGTH "${sha256}" sha256_length )
    if ( NOT sha256_length EQUAL 64 OR NOT sha256 MATCHES "^[0-9a-f]+$" )
      message( FATAL_ERROR "${origin}: entry '${label}' has a malformed \"sha256\": ${sha256}" )
    endif()
    set( cached "${MCCODE_EXTERNALS_CACHE}/sha256/${sha256}/${label}" )
    if ( EXISTS "${cached}" )
      file( SHA256 "${cached}" have )
      if ( have STREQUAL sha256 )
        set( ${out_var} "${cached}" PARENT_SCOPE )
        return()
      endif()
      file( REMOVE "${cached}" )
    endif()
  endif()

  # A pre-fetched copy provided by the builder wins over the network.
  if ( MCCODE_EXTERNALS_LOCAL AND EXISTS "${MCCODE_EXTERNALS_LOCAL}/${label}" )
    set( source "${MCCODE_EXTERNALS_LOCAL}/${label}" )
    message( STATUS "  ${label}: using local copy ${source}" )
  elseif ( MCCODE_EXTERNALS_OFFLINE )
    message( FATAL_ERROR
      "${origin}: MCCODE_EXTERNALS_OFFLINE is set, but '${label}' is not in the cache "
      "(${MCCODE_EXTERNALS_CACHE}) and MCCODE_EXTERNALS_LOCAL is \"${MCCODE_EXTERNALS_LOCAL}\". "
      "It would otherwise have been downloaded from ${url}" )
  else()
    if ( url STREQUAL "" )
      message( FATAL_ERROR "${origin}: entry '${label}' resolves to no URL, archive or local file." )
    endif()
    set( source "${cached}.download" )
    message( STATUS "  ${label}: downloading ${url}" )
    file( DOWNLOAD "${url}" "${source}"
          TIMEOUT ${MCCODE_EXTERNALS_TIMEOUT}
          TLS_VERIFY ON
          STATUS status
          LOG log )
    list( GET status 0 code )
    if ( NOT code EQUAL 0 )
      list( GET status 1 reason )
      file( REMOVE "${source}" )
      message( FATAL_ERROR "${origin}: failed to download '${label}' from ${url}: ${reason}\n${log}" )
    endif()
  endif()

  if ( NOT sha256 STREQUAL "" )
    file( SHA256 "${source}" have )
    if ( NOT have STREQUAL sha256 )
      if ( source STREQUAL "${cached}.download" )
        file( REMOVE "${source}" )
      endif()
      message( FATAL_ERROR
        "${origin}: checksum mismatch for '${label}' (from ${url})\n"
        "  expected sha256 ${sha256}\n"
        "  obtained sha256 ${have}\n"
        "Upstream content changed under a fixed reference, or the manifest is stale. "
        "Verify the change is intended before updating the manifest." )
    endif()
  endif()

  get_filename_component( cache_dir "${cached}" DIRECTORY )
  file( MAKE_DIRECTORY "${cache_dir}" )
  if ( source STREQUAL "${cached}.download" )
    file( RENAME "${source}" "${cached}" )
  else()
    configure_file( "${source}" "${cached}" COPYONLY )
  endif()
  set( ${out_var} "${cached}" PARENT_SCOPE )
endfunction()

# Unpack the archive described by <archive_json> once; return the directory
# its contents live in, after dropping "strip" leading path components.
function( _mcext_archive_root out_var archive_json origin )
  _mcext_get( url    "${archive_json}" url )
  _mcext_get( sha256 "${archive_json}" sha256 )
  _mcext_get( strip  "${archive_json}" strip )
  if ( strip STREQUAL "" )
    set( strip 1 )
  endif()

  get_filename_component( archive_name "${url}" NAME )
  if ( archive_name STREQUAL "" )
    set( archive_name "archive" )
  endif()
  _mcext_cache( archive "${archive_name}" "${url}" "${sha256}" "${origin}" )

  if ( sha256 STREQUAL "" )
    string( MD5 key "${url}" )
  else()
    string( TOLOWER "${sha256}" key )
  endif()
  set( extracted "${MCCODE_EXTERNALS_CACHE}/extract/${key}" )
  if ( NOT EXISTS "${extracted}/.mccode-extracted" )
    file( REMOVE_RECURSE "${extracted}" )
    file( MAKE_DIRECTORY "${extracted}" )
    message( STATUS "  ${archive_name}: unpacking" )
    file( ARCHIVE_EXTRACT INPUT "${archive}" DESTINATION "${extracted}" )
    file( TOUCH "${extracted}/.mccode-extracted" )
  endif()

  set( root "${extracted}" )
  if ( strip GREATER 0 )
    foreach( level RANGE 1 ${strip} )
      file( GLOB children "${root}/*" )
      list( REMOVE_ITEM children "${root}/.mccode-extracted" )
      list( LENGTH children count )
      if ( NOT count EQUAL 1 OR NOT IS_DIRECTORY "${children}" )
        message( FATAL_ERROR
          "${origin}: cannot strip ${strip} path component(s) from ${archive_name}: "
          "${root} does not hold exactly one directory. Set \"strip\" in the manifest's "
          "\"archive\" object to the correct depth (0 disables stripping)." )
      endif()
      set( root "${children}" )
    endforeach()
  endif()
  set( ${out_var} "${root}" PARENT_SCOPE )
endfunction()

# Read one manifest and stage its files under <stage_dir>/<subdir>.
# Appends "<relative install subdir>|<staged file>" records to out_var.
function( _mcext_process ext_file stage_dir subdir out_var )
  set( records "${${out_var}}" )
  get_filename_component( origin_name "${ext_file}" NAME )
  file( READ "${ext_file}" content )

  string( JSON top_type ERROR_VARIABLE error TYPE "${content}" )
  if ( error )
    message( FATAL_ERROR "${ext_file}: not valid JSON: ${error}" )
  endif()

  if ( top_type STREQUAL "ARRAY" )
    set( entries "${content}" )
    set( defaults "{}" )
  elseif ( top_type STREQUAL "OBJECT" )
    set( defaults "${content}" )
    _mcext_get( entries "${content}" files )
    if ( entries STREQUAL "" )
      message( FATAL_ERROR "${ext_file}: object manifest has no \"files\" array." )
    endif()
  else()
    message( FATAL_ERROR "${ext_file}: manifest must be a JSON array or object, found ${top_type}." )
  endif()

  _mcext_get( contribution "${defaults}" name )
  _mcext_get( version      "${defaults}" version )
  _mcext_get( license      "${defaults}" license )
  if ( contribution STREQUAL "" )
    get_filename_component( contribution "${ext_file}" NAME_WE )
  endif()
  set( banner "${contribution}" )
  if ( NOT version STREQUAL "" )
    string( APPEND banner " ${version}" )
  endif()
  if ( NOT license STREQUAL "" )
    string( APPEND banner " (${license})" )
  endif()
  message( STATUS "External contribution ${banner} <- ${origin_name}" )

  _mcext_get( archive_json "${defaults}" archive )
  set( archive_root "" )

  string( JSON entry_count LENGTH "${entries}" )
  if ( entry_count EQUAL 0 )
    message( WARNING "${ext_file}: manifest lists no files." )
  endif()
  math( EXPR last "${entry_count} - 1" )
  foreach( index RANGE 0 ${last} )
    if ( entry_count EQUAL 0 )
      break()
    endif()
    string( JSON entry GET "${entries}" ${index} )

    _mcext_get( name "${entry}" name )
    if ( name STREQUAL "" )
      message( FATAL_ERROR "${ext_file}: entry ${index} has no \"name\"." )
    endif()

    _mcext_get(     install_as "${entry}"             as )
    _mcext_get(     from       "${entry}"             from )
    _mcext_inherit( url        "${entry}" "${defaults}" url )
    _mcext_inherit( sha256     "${entry}" "${defaults}" sha256 )
    _mcext_inherit( base       "${entry}" "${defaults}" base )
    _mcext_inherit( git        "${entry}" "${defaults}" git )
    _mcext_inherit( entry_ver  "${entry}" "${defaults}" version )
    if ( install_as STREQUAL "" )
      set( install_as "${name}" )
    endif()
    if ( from STREQUAL "" )
      set( from "${name}" )
    endif()
    if ( IS_ABSOLUTE "${install_as}" OR install_as MATCHES "(^|/)\\.\\.(/|$)" )
      message( FATAL_ERROR "${ext_file}: entry '${name}' has an unsafe \"as\": ${install_as}" )
    endif()

    # A github.com "git" plus "version" is enough to address raw file content.
    if ( url STREQUAL "" AND base STREQUAL "" AND archive_json STREQUAL "" AND NOT git STREQUAL "" )
      string( REGEX REPLACE "/+$"    "" repo "${git}" )
      string( REGEX REPLACE "\\.git$" "" repo "${repo}" )
      if ( repo MATCHES "^https?://github\\.com/([^/]+)/([^/]+)$" AND NOT entry_ver STREQUAL "" )
        set( base "https://raw.githubusercontent.com/${CMAKE_MATCH_1}/${CMAKE_MATCH_2}/${entry_ver}/" )
      endif()
    endif()

    if ( NOT url STREQUAL "" )
      _mcext_cache( staged_source "${name}" "${url}" "${sha256}" "${ext_file}" )
    elseif ( NOT base STREQUAL "" )
      string( REGEX REPLACE "/+$" "" base "${base}" )
      _mcext_cache( staged_source "${name}" "${base}/${from}" "${sha256}" "${ext_file}" )
    elseif ( NOT archive_json STREQUAL "" )
      if ( archive_root STREQUAL "" )
        _mcext_archive_root( archive_root "${archive_json}" "${ext_file}" )
      endif()
      set( staged_source "${archive_root}/${from}" )
      if ( NOT EXISTS "${staged_source}" )
        message( FATAL_ERROR "${ext_file}: '${from}' is not present in the release archive." )
      endif()
      if ( NOT sha256 STREQUAL "" )
        string( TOLOWER "${sha256}" sha256 )
        file( SHA256 "${staged_source}" have )
        if ( NOT have STREQUAL sha256 )
          message( FATAL_ERROR
            "${ext_file}: checksum mismatch for '${from}' inside the release archive\n"
            "  expected sha256 ${sha256}\n  obtained sha256 ${have}" )
        endif()
      elseif ( NOT MCCODE_EXTERNALS_ALLOW_UNVERIFIED )
        message( FATAL_ERROR "${ext_file}: entry '${name}' has no \"sha256\"." )
      endif()
    else()
      message( FATAL_ERROR
        "${ext_file}: entry '${name}' names no source -- give it \"url\", or give the "
        "contribution a \"base\", an \"archive\", or a github.com \"git\" plus \"version\"." )
    endif()

    if ( subdir STREQUAL "" )
      set( relative "${install_as}" )
    else()
      set( relative "${subdir}/${install_as}" )
    endif()
    set( staged "${stage_dir}/${relative}" )
    configure_file( "${staged_source}" "${staged}" COPYONLY )

    get_filename_component( relative_dir "${relative}" DIRECTORY )
    list( APPEND records "${relative_dir}|${staged}" )
  endforeach()

  set( ${out_var} "${records}" PARENT_SCOPE )
endfunction()


# --- public entry point ----------------------------------------------------

function( mccode_install_externals )
  set( options NO_RECURSE )
  set( one_value DIRECTORY DESTINATION STAGE COMPONENT OUTPUT_VARIABLE )
  cmake_parse_arguments( EXT "${options}" "${one_value}" "" ${ARGN} )

  if ( EXT_UNPARSED_ARGUMENTS )
    message( FATAL_ERROR "mccode_install_externals: unexpected argument(s): ${EXT_UNPARSED_ARGUMENTS}" )
  endif()
  if ( NOT EXT_DIRECTORY OR NOT EXT_DESTINATION )
    message( FATAL_ERROR "mccode_install_externals: DIRECTORY and DESTINATION are both required." )
  endif()
  if ( EXT_OUTPUT_VARIABLE )
    set( ${EXT_OUTPUT_VARIABLE} "" PARENT_SCOPE )
  endif()
  if ( NOT ENABLE_EXTERNALS )
    return()
  endif()

  if ( IS_ABSOLUTE "${EXT_DIRECTORY}" )
    set( root "${EXT_DIRECTORY}" )
  else()
    set( root "${CMAKE_CURRENT_SOURCE_DIR}/${EXT_DIRECTORY}" )
  endif()
  get_filename_component( root "${root}" REALPATH )

  if ( EXT_NO_RECURSE )
    file( GLOB manifests LIST_DIRECTORIES false "${root}/*.ext" )
  else()
    file( GLOB_RECURSE manifests LIST_DIRECTORIES false "${root}/*.ext" )
  endif()
  if ( NOT manifests )
    return()
  endif()
  list( SORT manifests )

  set( stage "${EXT_STAGE}" )
  if ( stage STREQUAL "" )
    file( RELATIVE_PATH stage_key "${CMAKE_CURRENT_SOURCE_DIR}" "${root}" )
    if ( IS_ABSOLUTE "${stage_key}" OR stage_key MATCHES "^\\.\\." )
      # Outside this directory's source tree: name the stage after the leaf
      # directory, disambiguated by a digest of the full path.
      get_filename_component( stage_leaf "${root}" NAME )
      string( MD5 stage_hash "${root}" )
      string( SUBSTRING "${stage_hash}" 0 8 stage_hash )
      set( stage_key "${stage_leaf}-${stage_hash}" )
    endif()
    string( MAKE_C_IDENTIFIER "${stage_key}" stage_key )
    set( stage "${CMAKE_CURRENT_BINARY_DIR}/externals/${stage_key}" )
  endif()

  set( records "" )
  foreach( manifest ${manifests} )
    # Re-run configure when a manifest is edited, so that changing a pinned
    # version takes effect without a manual cmake invocation.
    set_property( DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${manifest}" )
    get_filename_component( manifest_dir "${manifest}" DIRECTORY )
    file( RELATIVE_PATH subdir "${root}" "${manifest_dir}" )
    if ( subdir STREQUAL "." )
      set( subdir "" )
    endif()
    _mcext_process( "${manifest}" "${stage}" "${subdir}" records )
  endforeach()

  # Group by install subdirectory so each destination gets a single rule.
  set( subdirs "" )
  set( staged_all "" )
  foreach( record ${records} )
    string( FIND "${record}" "|" split )
    string( SUBSTRING "${record}" 0 ${split} record_dir )
    math( EXPR value_at "${split} + 1" )
    string( SUBSTRING "${record}" ${value_at} -1 record_file )
    string( MAKE_C_IDENTIFIER "d_${record_dir}" key )
    if ( NOT DEFINED group_${key} )
      list( APPEND subdirs "${key}" )
      set( group_dir_${key} "${record_dir}" )
      set( group_${key} "" )
    endif()
    list( APPEND group_${key} "${record_file}" )
    list( APPEND staged_all "${record_file}" )
  endforeach()

  set( component_args "" )
  if ( EXT_COMPONENT )
    set( component_args COMPONENT "${EXT_COMPONENT}" )
  endif()
  foreach( key ${subdirs} )
    set( destination "${EXT_DESTINATION}" )
    if ( NOT group_dir_${key} STREQUAL "" )
      set( destination "${EXT_DESTINATION}/${group_dir_${key}}" )
    endif()
    install( FILES ${group_${key}} DESTINATION "${destination}" ${component_args} )
  endforeach()

  list( LENGTH staged_all staged_count )
  message( STATUS "Staged ${staged_count} external file(s) from ${EXT_DIRECTORY} into ${stage}" )
  if ( EXT_OUTPUT_VARIABLE )
    set( ${EXT_OUTPUT_VARIABLE} "${staged_all}" PARENT_SCOPE )
  endif()
endfunction()
