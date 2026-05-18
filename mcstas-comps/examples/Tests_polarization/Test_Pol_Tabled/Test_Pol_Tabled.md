# `Test_Pol_Tabled.instr` with inputs and tools
The instrument uses tabulated field-data for input, examples are:
* A flipping field `flipfield.dat` 
* A constant field `constfield.dat`

Further, a couple of remeshing tools are provided, allowing to resample an existing field-file to 'regular' binning, both with positional inputs of `infile` for the input file and `xsiz`, `ysiz`, `zsiz` for the resampling dimensions
* `remesh.m` function for Matlab users
* `remesh.py` script for Python users

When resampling for use with 'regular' interpolation (e.g. on GPU), esure that `xsiz`, `ysiz`, `zsiz` are equal. 
