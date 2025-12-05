# Install McStas 3.5.40 through conda-forge (macOS, Linux or Windows host)

* We a set of conda-packages for installing McStas 3.5.40.x through conda

## Get yourself a conda / mamba
Due to the complex [licensing situation](https://discuss.scientific-python.org/t/response-to-anaconda-switch-to-paid-plans/1395) with the commercial Anaconda ecosystem, we clearly recommend McStas users to start from an open-source entry-point such as
* [miniforge](https://github.com/conda-forge/miniforge) or
* [micromamba](https://mamba.readthedocs.io/en/latest/user_guide/micromamba.html)

## If you ALREADY have a conda (be it Anaconda, blah-conda or whatever)
* Initially, list mcstas versions available on conda-forge, just to be sure you pick them up:
* ```conda search mcstas --channel conda-forge```, should give you many lines of the form
* ```mcstas                         3.5.8      hc.31654_1  conda-forge   ```
* To install McStas with all needed dependencies, run
* ```conda create --name mcstas --channel conda-forge --channel nodefaults mcstas```

## If you DON'T have a conda already:

### Optionally create a dedicated environment and load it
* ```conda create --name mcstas ```
* ```conda activate mcstas```

### Install McStas, compilers, openmpi
* ```mamba install mcstas```

## Note for use on Windows
On Windows you will further need to install "c++ build tools for VS code" manually:

Depending on your needs and available hard drive space, you can either directly install VC-2019 using the [Visual Studio Build Tools 2019 installer](https://aka.ms/vs/16/release/vs_BuildTools.exe), or you can install both VC-2022 and VC-2019 using the [Visual Studio Build Tools 2022 installer](https://aka.ms/vs/17/release/vs_BuildTools.exe), making sure to check the optional box for "MSVC v142 - VS 2019 C++ x64/x86 build tools (v14.29)".

You are welcome to contact us in case of issues, e.g. via
*   [mcstas-users](mailto:mcstas-users@mcstas.org)
* by creating an issue on [the McCode issue tracker](https://github.com/mccode-dev/McCode/issues)
