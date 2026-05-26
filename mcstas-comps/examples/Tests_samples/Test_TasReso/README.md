# The `Test_TasReso` Instrument

*McStas: Testing the triple-axis resolution, use "reso.dat" output with mcresplot.py*

## Identification

- **Site:** Tests_samples
- **Author:** Tobias Weber (tweber@ill.fr)
- **Origin:** ILL
- **Date:** 30-Mar-2019

## Description

```text
Testing the triple-axis resolution, use "reso.dat" output with mcresplot.py

Triple-axis resolution test instrument,
forked from the Hercules McStas course:
https://code.ill.fr/tweber/hercules

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| src_lam | AA | Source mean wavelength | 4.5 |
| src_dlam | AA | Source wavelength spread | 1.0 |
| ki | AA^-1 | Monochromator ki | -1 |
| kf | AA^-1 | Analyser kf | -1 |
| sample_num |  | Selection of sample-model 0=vanadium, 1=single crystal, 2=resolution | 2 |
| mono_curvh | m | Monochromator horizontal curvature. -1: optimal, 0: flat | -1 |
| mono_curvv | m | Monochromator vertical curvature -1: optimal, 0: flat | -1 |
| ana_curvh | m | Analyzer horizontal curvature. -1: optimal, 0: flat | -1 |
| ana_curvv | m | Analyzer vertical curvature -1: optimal, 0: flat | -1 |
| coll_presample_div | arcmin | Collimation before sample | 30.0 |
| coll_postsample_div | arcmin | Collimation after sample | 30.0 |
| k_filter_cutoff | AA^-1 | Filter cutoff | 1.6 |

## Links

- [Source code](Test_TasReso.instr) for `Test_TasReso.instr`.

---
