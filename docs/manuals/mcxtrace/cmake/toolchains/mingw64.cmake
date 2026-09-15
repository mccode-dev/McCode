# the name of the target operating system
SET(CMAKE_SYSTEM_NAME "Windows")
SET(ARCH   "x86_64")

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix)
set(CMAKE_CXX_FLAGS "-fpermissive")
SET(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
SET(CMAKE_Fortran_COMPILER x86_64-w64-mingw32-gfortran)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /usr/x86_64-w64-mingw32)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Fetch ZLIB
set(MCPL_ENABLE_ZLIB IFAVAILABLE)

# Python and Perl suffixes
set(PYTHON_SUFFIX "py")
set(PERL_SUFFIX "pl")

# Set executable extension
set(EXE_SUFFIX   "exe")
set(CMAKE_EXECUTABLE_SUFFIX ".exe")

# Commands for running various external tools
set(BROWSER "start")
set(TERMINAL "start")
set(TOOLS_CC "gcc")
set(OACCFLAGS "-ta:multicore -DOPENACC")
set(MPICC "mpicc.bat")
set(MPIRUN "mpiexec.exe")
set(MPILIB "msmpi")
set(MCCODE_CFLAGS "-flto -O3 -mtune=native -fno-math-errno -ftree-vectorize -g -DNDEBUG -D_POSIX_SOURCE -std=c99 -lm")
set(EDITOR "start")

# NeXus location defaults
set(NEXUSINCLUDE "GETPATH(../miniconda3/Library/include/nexus)")
set(NEXUSLIB "GETPATH(../miniconda3/Library/lib)")

# HDFviewer
set(HDFVIEW "nexpy")

# gsl and xraylib locations
set(GSLFLAGS "-IGETPATH(../miniconda3/Library/include/) -Wl,-rpath,GETPATH(../miniconda3/Library/lib) -LGETPATH(../miniconda3/Library/lib) -lgsl -lgslcblas")
set(XRLFLAGS "-IGETPATH(../miniconda3/Library/include/) -Wl,-rpath,GETPATH(../miniconda3/Library/lib) -LGETPATH(../miniconda3/Library/lib) -lxrl")
set(MCCODE_BUILD_WINDOWS_MINGW "yes")
