# the name of the target operating system
SET(CMAKE_SYSTEM_NAME "Linux")
SET(ARCH   "arm64")

# Commands for running various external tools
set(BROWSER "xdg-open")
set(TERMINAL "x-terminal-emulator -e")
set(TOOLS_CC "gcc")
set(MPICC "mpicc")
set(MPILIB "mpi")
set(OACCFLAGS "-fast -Minfo=accel -acc=gpu -gpu=managed -DOPENACC")
set(MPIRUN "mpirun")
# Packages are built before libhdf5-dev is installed: set the
# full flags. Debian's serial HDF5 is in /usr/include/hdf5/serial, linked as libhdf5_serial
set(NEXUSFLAGS "-Wl,-rpath,/usr/lib -L/usr/lib -DUSE_NEXUS -DNX_COMPRESSION=NX_COMP_LZW_LVL1 -lhdf5_serial -I/usr/include/hdf5/serial")
set(MCCODE_CFLAGS "-fno-PIC -fPIE -flto -O3 -mtune=native -march=native -fno-math-errno -ftree-vectorize -g -DNDEBUG -D_POSIX_SOURCE -std=c99 -lm")
set(EDITOR "gedit")
