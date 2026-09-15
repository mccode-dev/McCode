# the name of the target operating system
SET(CMAKE_SYSTEM_NAME "FreeBSD")
SET(ARCH   "i386")

# Commands for running various external tools
set(BROWSER "xdg-open")
set(TERMINAL "xterm -e")
set(TOOLS_CC "clang")
set(MPICC "mpicc")
set(MPILIB "mpi")
set(MPIRUN "mpirun")
set(MCCODE_CFLAGS "-fno-PIC -fPIE -flto -O3 -mtune=native -march=native -fno-math-errno -ftree-vectorize -g -DNDEBUG -D_POSIX_SOURCE -std=c99 -lm")
set(EDITOR "gedit")
