# the name of the target operating system
set(CMAKE_SYSTEM_NAME "noarch")
set(ARCH   "noarch")

# Commands for running various external tools
set(BROWSER "xdg-open")
set(TOOLS_CC "cc")
set(MPICC "mpicc")
set(MPIRUN "mpirun")
set(MPILIB "mpi")
set(MCCODE_CFLAGS "-fno-PIC -fPIE -flto -O3 -mtune=native -march=native -fno-math-errno -ftree-vectorize -g -DNDEBUG -D_POSIX_SOURCE -std=c99 -lm")
set(EDITOR "gedit")
