# Installation of McStas 3.5.40 on Windows 64 bit systems - using WSL
*(WSL is the Windows Subsystem for Linux, aka. bash on Ubuntu on Windows)*


## Enable WSL / Ubuntu:
* Open the Windows store
* Search for Ubuntu
* Install e.g. Ubuntu 22.04 LTS (other Linuxes are also
available, but we recommend Ubuntu)
* Click Launch to run the app and follow on-screen instructions
* To open it again later, simply issue bash in a terminal or through
the start menu

## Install the McStas 3.5.40 Debian packages
* Follow the
  [normal Debian installation instructions](../../Linux/debian/README.md)
  - essentially a matter of sudo apt-get install mcstas-suite-python

## For WSL 2.0: Use WSLg
* Please try to upgrade WSL to v. 2.0 or later which should install [WSLg graphical support (X11)](https://github.com/microsoft/wslg)

## For WSL 1: Install Xming or another X11 server application
* Download and install Xming via https://sourceforge.net/projects/xming/
* Windows store also provides a $-ware called X410 from "Choung Networks"

## For WSL 1: Optionally add a few commands at the end of your .bashrc:
```bash
cd $HOME
nano .bashrc
```
At the end, add the lines below, substituting pwill for  your windows
user name
```bash
# cd to the home dir of your windows user
cd /mnt/c/Users/pwill
# Add the DISPLAY variable to talk to Xming
export DISPLAY=:0.0
```
if you installed wsl version 2 your ubuntu session has a unique ip
```
# Add the DISPLAY variable to talk to Xming for wsl version 2
export DISPLAY=$(awk '/nameserver / {print $2; exit}' /etc/resolv.conf 2>/dev/null):0
export LIBGL_ALWAYS_INDIRECT=1
```

## Start mcgui
* Ensure your X11 server is running
* For WSL 1: Start bash and ensure the DISPLAY variable is set (export
DISPLAY=:0.0)
* Issue the command mcgui

## Configuring McStas to use the Windows browser
* Ensure the package ubuntu-wsl (or equivalent) is installed
* Set the BROWSER="wslview" in your /usr/share/mcstas/tools/Python/mccodelib/mccode_config.py

## In case of issues
Please report any trouble with the repository to [mcstas-users](mailto:mcstas-users@mcstas.org)

