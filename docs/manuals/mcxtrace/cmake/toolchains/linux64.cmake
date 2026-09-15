# the name of the target operating system
SET(CMAKE_SYSTEM_NAME "Linux")
SET(ARCH   "amd64")

# Commands for running various external tools
set(BROWSER "xdg-open")
set(TERMINAL "xterm -e")
set(TOOLS_CC "gcc")
set(MPICC "mpicc")
set(OACCFLAGS "-fast -Minfo=accel -acc=gpu -gpu=managed -DOPENACC")
set(MPILIB "mpi")
set(MPIRUN "mpirun")
set(MCCODE_CFLAGS "-g -O2 -lm -DNDEBUG -std=c99 -D_POSIX_SOURCE")
set(EDITOR "gedit")
