## Install McStas 3.5.40 On Debian class systems (including Ubuntu, mint etc.):
The packages have been tested to work correctly on Ubuntu 24.04.

# Add the McCode repository
After following the below steps your package manager should now be aware of mcstas
```bash
cd /etc/apt/sources.list.d
sudo -E wget https://packages.mccode.org/debian/mccode.list
sudo apt-get update
```

# Debian only:
On Debian you will further have to install the non-free repository to have access to all McStas tool parts. See https://wiki.debian.org/SourcesList

# Look for McStas packages to install
```bash
mcstas@debian:~$ apt-cache search mcstas | grep -v 2.
mcstas - mcstas built using CMake
mcstas-comps - mcstas-comps built using CMake
mcstas-manuals - mcstas_manuals built using CMake
mcstas-suite-python - A metapackage for McStas + python tools
mcstas-tools-matlab-mcplot - matlab-tools-mcplot built using CMake
mcstas-tools-python-mccodelib - python-tools-mccodelib built using CMake
mcstas-tools-python-mcdisplay-mantid - python-tools-mcdisplay-mantid built using CMake
mcstas-tools-python-mcdisplay-pyqtgraph - python-tools-mcdisplay-pyqtgraph built using CMake
mcstas-tools-python-mcdisplay-webgl - python-tools-mcdisplay-webgl built using CMake
mcstas-tools-python-mcgui - python-tools-mcgui built using CMake
mcstas-tools-python-mcplot-pyqtgraph - python-tools-mcplot-pyqtgraph built using CMake
mcstas-tools-python-mcrun - python-tools-mcrun built using CMake
```
The meta-package mcstas-suite-python (or mcstas-suite-python-ng)
allows you to install mcstas 3.5.40 with tools (mcrun/mcplot etc.) by
the simple apt-get command
```bash
sudo apt-get install mcstas-suite-python
```

# Important note wrt. Debian packages:
If you install both of mcstas and mcxtrace on the same Debian/Ubuntu system, you will get a collision for the file `/usr/bin/cif2hkl`. As a workaround you may allow joint installation via overriding `cif2hkl`:
  `sudo apt-get -f install  -o Dpkg::Options::="--force-overwrite"`

# Using mcdoc on modern Ubuntu systems
Ubuntu is shipping its browsers as "snap" packages, meaning that
e.g. the components can not be browsed from /usr/share/mcstas/.

To fix this we propose to switch your browser to a proper apt based
installation, in this example firefox:

Remove the snap-based firefox:
```
sudo snap remove firefox
```
Add the official mozilla-built firefox instead:
```
sudo add-apt-repository ppa:mozillateam/ppa
```
Set priorities to always prefer this firefox package:
```
echo '
Package: *
Pin: release o=LP-PPA-mozillateam
Pin-Priority: 1001
' | sudo tee /etc/apt/preferences.d/mozilla-firefox
```
Set up unattended upgrades of firefox
```
echo 'Unattended-Upgrade::Allowed-Origins::
"LP-PPA-mozillateam:${distro_codename}";' | sudo tee
/etc/apt/apt.conf.d/51unattended-upgrades-firefox
```
Finally, install firefox from the mozilla-channels
```
sudo apt install firefox
```

## In case of issues
Please report any trouble with the repository to [mcstas-users](mailto:mcstas-users@mcstas.org)


