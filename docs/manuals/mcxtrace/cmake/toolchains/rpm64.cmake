# the name of the target operating system
SET(CMAKE_SYSTEM_NAME "Linux")
SET(ARCH   "x86_64")

# Commands for running various external tools
set(BROWSER "xdg-open")
set(TERMINAL "gnome-terminal -e")
set(TOOLS_CC "gcc")
set(MPICC "mpicc")
set(OACCFLAGS "-fast -Minfo=accel -acc=gpu -gpu=managed -DOPENACC")
set(MPIRUN "mpirun")
set(MPILIB "mpi")
set(MCCODE_CFLAGS "-fno-PIC -fPIE -flto -O3 -mtune=native -march=native -fno-math-errno -ftree-vectorize -g -DNDEBUG -D_POSIX_SOURCE -std=c99 -lm")
set(EDITOR "gedit")
