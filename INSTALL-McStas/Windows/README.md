# Installation of McStas 3.7.6 on Windows 64 bit Intel systems (some support for arm64)
## *IMPORTANT: active internet connection required during installation*

* Please use either of:
  1. **Recommended:**
    * As **Administrator**, run our new [`install_msvc_for_conda.bat`](install_msvc_for_conda.bat) batch script to install MSVC and required packages to your system. Alternatively, [install MSVC manually](VScode_manual_install)
    * Use the [`mcstas-conda`](mcstas-conda.bat) batch script to install the latest mcstas from `conda-forge` 
    * Once install has completed, please use the `mcstas-shell-conda` or `mcgui-conda` shortcuts on the desktop. The first one issues a McStas terminal, the latter start `mcgui` directly also.
    * Option 1 functions also on `arm64` Windows, but will require installation of Visual Studio including both arm64 and x64_64 build tools for c++.
  2. **New MSVC-bundle solution:**
    * As **Administrator**, run our new  [`install_msvc_for_conda.bat`](install_msvc_for_conda.bat) batch script to install MSVC and required packages to your system. Alternatively, [install MSVC manually](VScode_manual_install)
    * Please remove any existing MinGW-based version of McStas 3.7.6 before proceeding!
    * install the new MSVC based [McStas 3.7.6 MSVC metapackage](https://download.mcstas.org/mcstas-3.7.6/Windows/MSVC/McStas-Metapackage-3.7.6-MSVC-win64.exe)
    * Option 2 should be fully functional on `arm64` processors.
  3. **WSL:**
    * Use the directions available in [WSL](WSL/README.md) to install the "Windows subsystem for Linux" and run the Linux Debian binaries there. 
    * Option 3 is fully functional on `arm64` processors.
  4. **Legacy MinGW-bundle solution:**
    * Please remove any MSVC-based version of McStas 3.7.6 before proceeding!
    * install the legacy-style MinGW based [McStas 3.7.6 MinGW metapackage](https://download.mcstas.org/mcstas-3.7.6/Windows/MinGW/McStas-Metapackage-3.7.6-MinGW-win64.exe)
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
in a cmd.exe shell and start the McStas installer from there	

## In case of issues
Please report any trouble with the repository to [mcstas-users](mailto:mcstas-users@mcstas.org)

