## Install McXtrace 3.5.40 On Debian class systems (including Ubuntu, mint etc.):
The packages have been tested to work correctly on Ubuntu 24.04.

# Add the McCode repository
After following the below steps your package manager should now be aware of mcxtrace
```bash
cd /etc/apt/sources.list.d
sudo -E wget https://packages.mccode.org/debian/mccode.list
sudo apt-get update
```

# Debian only:
On Debian you will further have to install the non-free repository to have access to all McXtrace tool parts. See https://wiki.debian.org/SourcesList

# Look for McXtrace packages to install
```bash
mcxtrace@debian:~$ apt-cache search mcxtrace | grep -v 2.
mcxtrace - mcxtrace built using CMake
mcxtrace-comps - mcxtrace-comps built using CMake
mcxtrace-manuals - mcxtrace_manuals built using CMake
mcxtrace-suite-python - A metapackage for McXtrace + python tools
mcxtrace-tools-matlab-mxplot - matlab-tools-mxplot built using CMake
mcxtrace-tools-python-mccodelib - python-tools-mccodelib built using CMake
mcxtrace-tools-python-mxdisplay-pyqtgraph - python-tools-mxdisplay-pyqtgraph built using CMake
mcxtrace-tools-python-mxdisplay-webgl - python-tools-mxdisplay-webgl built using CMake
mcxtrace-tools-python-mxgui - python-tools-mxgui built using CMake
mcxtrace-tools-python-mxplot-pyqtgraph - python-tools-mxplot-pyqtgraph built using CMake
mcxtrace-tools-python-mxrun - python-tools-mxrun built using CMake
```
The meta-package mcxtrace-suite-python (or mcxtrace-suite-python-ng)
allows you to install mcxtrace 3.5.40 with tools (mcrun/mcplot etc.) by
the simple apt-get command
```bash
sudo apt-get install mcxtrace-suite-python
```

# Important note wrt. Debian packages:
If you install both of mcstas and mcxtrace on the same Debian/Ubuntu system, you will get a collision for the file `/usr/bin/cif2hkl`. As a workaround you may allow joint installation via overriding `cif2hkl`:
  `sudo apt-get -f install  -o Dpkg::Options::="--force-overwrite"`

# Using mcdoc on modern Ubuntu systems
Ubuntu is shipping its browsers as "snap" packages, meaning that
e.g. the components can not be browsed from /usr/share/mcxtrace/.

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
Please report any trouble with the repository to [mcxtrace-users](mailto:mcxtrace-users@mcxtrace.org)


