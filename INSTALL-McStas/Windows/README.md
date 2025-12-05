# Installation of McStas 3.5.40 on Windows 64 bit Intel systems (some support for arm64)
## *IMPORTANT: active internet connection required during installation*

* Please use either of:
  1. **Recommended:**
    * Install [Visual Studio including c/c++ support](https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170#download-and-install-the-tools). Any
      recent version of Visual Studio itself should work. Use the
      "Visual Studio Installer" and [modify settings](install-screenshot1.png) to selecect at minumum "Desktop development with c++" specifying to [include VS2019 build tools and CLI support](install-screenshot2.png). (It is OK to include build tools such as VS2022 or VS2026, but VS2019 **must** be present also. 
    * Use the [mcstas-conda](mcstas-conda.bat) batch script to install the latest mcstas from `conda-forge` 
    * Once install has completed, please use the `mcstas-shell` shortcut on the desktop - issue terminal command `mcgui` to start the McStas GUI. 
    * Option 1 *may* function `arm64` Windows, but will require installation of Visual Studio including both arm64 and x64_64 build tools for c++.
  2. **Legacy:**
    * install the legacy-style MinGW based [McStas 3.5.40 metapackage](https://download.mcstas.org/mcstas-3.5.40/Windows/McStas-Metapackage-3.5.40-win64.exe)
    * Option 2 should be fully functional on `arm64` processors.
  3. **WSL:**
    * Use the directions available in [WSL](WSL/README.md) to install the "Windows subsystem for Linux" and run the Linux Debian binaries there. 
    * Option 3 is fully functional on `arm64` processors.
	
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

