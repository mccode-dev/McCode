# Installation of McXtrace 3.7.9 on Windows 64 bit Intel systems (some support for arm64)
## *IMPORTANT: active internet connection required during installation*

###Please use either of:###

**OPTION 1: Recommended:**
* As **Administrator**, run our new [`install_msvc_for_conda.bat`](install_msvc_for_conda.bat) batch script to install MSVC and required packages to your system. Alternatively, [install MSVC manually](VScode_manual_install)
* Use the [`mcxtrace-conda`](mcxtrace-conda.bat) batch script to install the latest mcxtrace from `conda-forge` 
* Once install has completed, please use the `mcxtrace-shell-conda` or `mxgui-conda` shortcuts on the desktop. The first one issues a McXtrace terminal, the latter start `mxgui` directly also. 
* Option 1 functions also on `arm64` Windows, but will require installation of Visual Studio including both arm64 and x64_64 build tools for c++.

**OPTION 2: New MSVC-bundle solution:**
* As **Administrator**, run our new  [`install_msvc_for_conda.bat`](install_msvc_for_conda.bat) batch script to install MSVC and required packages to your system. Alternatively, [install MSVC manually](VScode_manual_install)
* Please remove any MinGW-based version of McXtrace 3.7.9 before proceeding!
* install the new MSVC based [McXtrace 3.7.9 MSVC metapackage](https://download.mcxtrace.org/mcxtrace-3.7.9/Windows/MSVC/McXtrace-Metapackage-3.7.9-MSVC-win64.exe)
* Option 2 should be fully functional on `arm64` processors.

**OPTION 3: WSL**
* Use the directions available in [WSL](WSL/README.md) to install the "Windows subsystem for Linux" and run the Linux Debian binaries there.  
* Option 3 is fully functional on `arm64` processors.

**OPTION 4: Legacy MinGW-bundle solution:**
* Please remove any MSVC-based version of McXtrace 3.7.9 before proceeding!
* install the legacy-style MinGW based [McXtrace 3.7.9 metapackage](https://download.mcxtrace.org/mcxtrace-3.7.9/Windows/MinGW/McXtrace-Metapackage-3.7.9-MinGW-win64.exe)
* Option 4 should be fully functional on `arm64` processors.
 
If you are `conda-forge` knowledgable already, you may also simply install a new environment as explained in [conda-forge](../conda/README.md)

## Internet access required:
* If you are behind a proxy server, please use the commands
	```bash
		set HTTP_PROXY=http://your_proxy_ip:port
	```
	```bash
		set HTTPS_PROXY=https://your_proxy_ip:port
	```
in a cmd.exe shell and start the McXtrace installer from there	

## In case of issues
Please report any trouble with the repository to [mcxtrace-users](mailto:mcxtrace-users@mcxtrace.org)

