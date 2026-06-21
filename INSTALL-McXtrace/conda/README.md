# Install McXtrace 3.7.6 through conda-forge (macOS, Linux or Windows host)

* We a set of conda-packages for installing McXtrace 3.7.6.x through conda

## Get yourself a conda / mamba
Due to the complex [licensing situation](https://discuss.scientific-python.org/t/response-to-anaconda-switch-to-paid-plans/1395) with the commercial Anaconda ecosystem, we clearly recommend McXtrace users to start from an open-source entry-point such as
* [micromamba](https://mamba.readthedocs.io/en/latest/user_guide/micromamba.html)

## Once micromamba is installed installed:
* Initially, you may list mcxtrace versions available on conda-forge, just to be sure you pick them up:
* ```micromamba search mcxtrace --channel conda-forge```, should give you many lines of the form
* ```mcxtrace                         3.7.6      hc.31654_1  conda-forge   ```
* To install McStas with all needed dependencies in the environment `mcxtrace`, run
* ```micromamba create --name mcxtrace --channel conda-forge --channel nodefaults mcxtrace```
* The `--name` argument defines the environment to create for the mcxtrace installation
* A specific version may be installed via e.g. ```micromamba create --name mcxtrace --channel conda-forge --channel nodefaults mcxtrace=3.7.6```
* For some conda variants you may need an `env` in the  syntax like this: `conda env create ...` 

## Note for use on Windows
On Windows you will further need to install "c++ build tools for VS code" manually:

Depending on your needs and available hard drive space, you can either directly install VC-2019 using the [Visual Studio Build Tools 2019 installer](https://aka.ms/vs/16/release/vs_BuildTools.exe), or you can install both VC-2022 and VC-2019 using the [Visual Studio Build Tools 2022 installer](https://aka.ms/vs/17/release/vs_BuildTools.exe), making sure to check the optional box for "MSVC v142 - VS 2019 C++ x64/x86 build tools (v14.29)".

You are welcome to contact us in case of issues, e.g. via
*   [mcxtrace-users](mailto:mcxtrace-users@mcxtrace.org)
* by creating an issue on [the McCode issue tracker](https://github.com/mccode-dev/McCode/issues)
