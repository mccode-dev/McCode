include(FetchContent)

function(git_fetch package min_version fetch_version_or_branch source required scoped_build_params )
    # If provided a bare version number check for if it is already installed locally
    if (min_version MATCHES "^[0-9]+\.([0-9]+\.)*[0-9]+$")
      if (${required})
          find_package(${package} ${min_version} REQUIRED)
      else()
          find_package(${package} ${min_version} QUIET)
      endif()
    endif()

    if (${${package}_FOUND})
        message(STATUS "Found system ${package}")
    else()
        message(STATUS "Fetch ${package} ${fetch_version} from ${source}")
        FetchContent_Declare(${package} GIT_REPOSITORY ${source} GIT_TAG ${fetch_version})
        FetchContent_GetProperties(${package})
        if ( scoped_build_params )
          while( scoped_build_params )
            list(POP_FRONT scoped_build_params parname parval)
            message(STATUS "  Building ${package} with ${parname}=${parval}")
            set(${parname} "${parval}")
          endwhile()
        endif()
        if (NOT "${package}_POPULATED")
            FetchContent_Populate(${package})
            add_subdirectory("${${package}_SOURCE_DIR}" "${${package}_BINARY_DIR}")
        endif()
        set(${package}_FOUND ON PARENT_SCOPE)
        set("${package}_SOURCE_DIR" "${${package}_SOURCE_DIR}" PARENT_SCOPE)
        set("${package}_BINARY_DIR" "${${package}_BINARY_DIR}" PARENT_SCOPE)
    endif()
endfunction()

# Lighter-weight sibling to git_fetch(): fetches a repo's source tree via
# FetchContent, but never add_subdirectory()'s it and never runs a
# find_package() pre-check. Use this for "contributed" repos whose CMakeLists
# we do not want to build (e.g. because it has its own, possibly higher,
# cmake_minimum_required, or because we only want to lift specific files out
# of it -- comps/instruments/library snippets -- rather than build it as a
# CMake subproject. Honors the standard FetchContent overrides, e.g.
# -D<package>_SOURCE_DIR=/local/checkout or FETCHCONTENT_SOURCE_DIR_<PACKAGE>=...
function(content_fetch package version_or_tag source)
    message(STATUS "Fetch ${package} ${version_or_tag} from ${source}")
    FetchContent_Declare(${package} GIT_REPOSITORY ${source} GIT_TAG ${version_or_tag} GIT_SHALLOW TRUE)
    FetchContent_GetProperties(${package})
    if (NOT ${package}_POPULATED)
        FetchContent_Populate(${package})
    endif()
    set("${package}_SOURCE_DIR" "${${package}_SOURCE_DIR}" PARENT_SCOPE)
    set("${package}_BINARY_DIR" "${${package}_BINARY_DIR}" PARENT_SCOPE)
endfunction()
