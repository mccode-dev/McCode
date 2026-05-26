# The `ATHENA_1sh_W1_OA` Instrument

*McXtrace: Single shell model of the ATHENA SPO-optic in use as telescope.*

## Identification

- **Site:** AstroX_ESA
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk> & Desiree D. M. Ferreira <desiree@space.dtu.dk> (email)
- **Origin:** DTU Physics/DTU Space
- **Date:** 12/12/2016

## Description

```text
A model of the ATHENA-telescope using just a single shell as optical element.
That means to make use of this instrument
it is necessary to run a series of simulation while varying the input parameter shellnumber.

The model needs as input a file geomfile, which contains (in ascii) tabled details about the geometry of the shells.
The data in the geomfile is assumed to be in the format:
#row  L/m  rad_h/m  rad_m/m  rad_p/m  width/m
...
row: running index for the rows/rings
L: The length of the plates for this ring
rad_h: The radius at the "hyperbolic" end of the optic. At the detector end.
rad_m: The radius at the midpoint of the optic. This is the reference.
rad_p: The radius at the "parabolic" and of the optic. At the source end.

Example: ATHENA_1sh_W1_OA.instr shellnumber=1
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
| reflectivity |  | Data file containing reflectivities (such as from IMD) | "mirror_coating_unity.txt" |
| E0 | keV | Central energy of X-rays | 5 |
| dE | keV | Half spread of energy spectrum to be emitted from source | 0.001 |
| shellnumber |  | The row number for the miror module. This defines the shell. | 1 |
| geomfile |  | File which contains the geometry of the pores (i.e. radii,lengths) | "ATHENA_rings_1_20.dat" |

## Links

- [Source code](ATHENA_1sh_W1_OA.instr) for `ATHENA_1sh_W1_OA.instr`.
- <a href="http://www.cosmos.esa.int/web/athena">The ATHENA web pages @ ESA</a>

---
