# The `NuSTAR_1shell_con` Instrument

*McXtrace: Single shell model of the NuSTAR-optic in use as telescope.*

## Identification

- **Site:** AstroX_ESA
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk> & Desiree D. M. Ferreira <desiree@space.dtu.dk> (email)
- **Origin:** DTU Physics/DTU Space
- **Date:** 12/12/2016

## Description

```text
A model of the NuSTAR-telescope using just a single shell as optical element.
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
width: pore width ?

Example: NuSTAR_1shell_con.instr shellnumber=1
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| FL | m | The focal length of the optical system | 10.12 |
| FLd | m | Detectors distance | 0 |
| optics_dist | m | The distance between souce and optic. In space this would be quite large :-). | 10 |
| SRC_POS_X | m | Displacement of source along X | 0 |
| SRC_POS_Y | m | Displacement of source along Y | 0 |
| offaxis_angle | arcmin | Angle of collimated light from source | 0 |
| drx |  |  | 0 |
| dry |  |  | 0 |
| drz |  |  | 0 |
| reflectivity |  | Data file containing reflectivities (such as from IMD) | "mirror_coating_unity.txt" |
| E0 | keV | Central energy of X-rays | 5 |
| dE | keV | Half spread of energy spectrum to be emitted from source | 0.001 |
| shellnumber |  | The row number for the miror module. This defines the shell. | 0 |
| parabolic_datafile |  |  | "om_con_1a_110901_t1.txt" |
| hyperbolic_datafile |  |  | "om_con_3a_110901_t1.txt" |

## Links

- [Source code](NuSTAR_1shell_con.instr) for `NuSTAR_1shell_con.instr`.
- <a href="http://www.cosmos.esa.int/web/athena">The ATHENA web pages @ ESA</a>

---
