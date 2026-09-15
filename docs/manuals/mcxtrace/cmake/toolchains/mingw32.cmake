# the name of the target operating system
SET(CMAKE_SYSTEM_NAME "Windows")
SET(ARCH   "i386")

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
set(CMAKE_CXX_FLAGS "-fpermissive")
SET(CMAKE_RC_COMPILER i686-w64-mingw32-windres)
SET(CMAKE_Fortran_COMPILER i686-w64-mingw32-gfortran)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /usr/i686-w64-mingw32)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Avoid ZLIB
set(BUILD_WITHZLIB OFF)

# Python and Perl suffixes
set(PYTHON_SUFFIX "py")
set(PERL_SUFFIX "pl")

# Set executable extension
set(EXE_SUFFIX   "exe")

# Commands for running various external tools
set(BROWSER "start")
set(TERMINAL "start")
set(TOOLS_CC "gcc")
set(OACCFLAGS "-ta:multicore -DOPENACC")
set(MPICC "mpicc.bat")
set(MPIRUN "mpiexec.exe")
set(MPILIB "msmpi")
set(MCCODE_CFLAGS "-fno-PIC -fPIE -flto -O3 -mtune=native -march=native -fno-math-errno -ftree-vectorize -g -DNDEBUG -D_POSIX_SOURCE -std=c99 -lm")
set(EDITOR "start")
