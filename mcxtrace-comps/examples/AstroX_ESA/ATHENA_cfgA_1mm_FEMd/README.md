# The `ATHENA_cfgA_1mm_FEMd` Instrument

*McXtrace: Single pore version of the ATHENA SPO-optic in use as telescope.*

## Identification

- **Site:** AstroX_ESA
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk> & Desiree D. M. Ferreira <desiree@space.dtu.dk> (email)
- **Origin:** DTU Physics/DTU Space
- **Date:** 12/12/2016

## Description

```text
A model of the ATHENA-telescope using just a single mirror module as optical element. That means to make use of this instrument
it is necessary to run a series of simulation while varying the input parameter porenumber.
At present, the porenumber really means mirror module number.

The model needs as input a set of files: A Mirror Module definition file and a ring definition file.
The former contains overall definitions of mirror modules and their positions - the latter contains details about each plate.
There is some redundancy between the two files. The latter will be rendered unnecessary in a later version.
An example of mmdef_file is "MM_Definitions-cfgA.csv" which is distributed by ESA.
An example of ringfile is "ref_design.dat" which is taken from the ATHENA_reference design white-paper.
In addition A displacement data file may be specified.

Reflectivity may be modelled using a datafile. In this telscope model only the top (intentionally reflecting) surface
is given a data-file. However, the MM-components can cope with non-zero reflectivity for side-walls and bottom surfaces.
The reflectivity datafile follows a simple ascii-format with 6 header lines the define the ranges in energy E and grazing
angle theta, followed by a 2D-data block with reflectivity numbers. This is expected to be substituted for a 1D-parametrization '
in q, to avoid overly lagre data-files.

Example: ATHENA_cfgA_1mm_FEMd.instr porenumber=3
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| FL | m | The focal length of the optical system | 12 |
| optics_dist | m | The distance between souce and optic. In space this would be quite large :-). | 10 |
| XWidth | m | The width of the user detector default is that of the ATHENA WFI large area detector | 0.13312 |
| YHeight | m | The height of the user detector default is that of the ATHENA WFI large area detector | 0.13312 |
| NX |  | Number of pixels along X in the user detector | 1024 |
| NY |  | Number of pixels along Y in the user detector. | 1024 |
| offaxis_angle | arcmin | Angle of collimated light from source | 0 |
| dPx | m | Offset/displacement of parabolic pore along x from its theoretical position. | 0 |
| dPy | m | Offset/displacement of parabolic pore along y from its theoretical position. | 0 |
| dPz | m | Offset/displacement of parabolic pore along z from its theoretical position. | 0 |
| dPrx | arcsec | Rotational misalignment of parabolic pore around x. | 0 |
| dPry | arcsec | Rotational misalignment of parabolic pore around y. | 0 |
| dPrz | arcsec | Rotational misalignment of parabolic pore around z. | 0 |
| dHx | m | Offset/displacement of hyperbolic pore along x from its theoretical position. | 0 |
| dHy | m | Offset/displacement of hyperbolic pore along y from its theoretical position. | 0 |
| dHz | m | Offset/displacement of hyperbolic pore along z from its theoretical position. | 0 |
| dHrx | arcsec | Rotational misalignment of hyperbolic pore around x. | 0 |
| dHry | arcsec | Rotational misalignment of hyperbolic pore around y. | 0 |
| dHrz | arcsec | Rotational misalignment of hyperbolic pore around z. | 0 |
| reflectivity |  | Data file containing reflectivities (such as from IMD) | "mirror_coating_unity.txt" |
| default_reflec |  | Default reflectivity value to use if no reflectivity file is given. | 0 |
| E0 | keV | Central energy of X-rays | 5 |
| dE | keV | Half spread of energy spectrum to be emitted from source | 0.001 |
| porenumber |  | Actually the mirror module number. | 1 |
| mmdef_file |  | File containing the positions and overall geometry of Mirror Modules. | "MM_Definitions-cfgA.csv" |
| ringfile |  | File which contains deatiled plate descriptions. | "ref_design_breaks.txt" |
| disp_file |  | Fiel that conatins mirror module displacements | "none" |
| disp_type |  | Displacement type | 0 |
| Hyper |  | If non-zero the secondary mirror (hyperbolic) is active. Useful for debugging. | 1 |
| Para |  | If non-zero the primary mirror is acive (parabolic) is active. Useful for debugging. | 1 |
| lists |  | If non-zero drop event mode monitr are active. Turn-off to save disk-space. | 0 |

## Links

- [Source code](ATHENA_cfgA_1mm_FEMd.instr) for `ATHENA_cfgA_1mm_FEMd.instr`.
- <a href="http://www.cosmos.esa.int/web/athena">The ATHENA web pages @ ESA</a>

---
