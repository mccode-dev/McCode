# Manual installation of VScode compiler support for McStas/McXtrace via conda 
## *IMPORTANT: active internet connection required during installation*

* Install [Visual Studio including c/c++ support](https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170#download-and-install-the-tools). Any
recent version of Visual Studio itself should work. 
* Use the "Visual Studio Installer" and [modify settings (click for screenshot)](install-screenshot1.png) to selecect at minumum "Desktop development with c++" specifying to [MSVC v143 (2022) plus CLI support (click for screenshot)](install-screenshot2.png). 
* It should be OK to include later build tools such as VS2026, but VS2022 **must** be present also. 

