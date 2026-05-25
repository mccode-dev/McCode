# The `ATHENA_1pore` Instrument

*McXtrace: Single pore version of the ATHENA SPO-optic in use as telescope.*

## Identification

- **Site:** AstroX_ESA
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk> & Desiree D. M. Ferreira <desiree@space.dtu.dk> (email)
- **Origin:** DTU Physics/DTU Space
- **Date:** 12/12/2016

## Description

```text
A model of the ATHENA-telescope using just a single pore as optical element. That means to make use of this instrument
it is necessary to run a series of simulation while varying the input parameter porenumber.
At present, the porenumber really means mirror module number, and only the central pore is used to be representative
module.

The model needs as input two files positionfile and geomfile, which contain (in ascii) tabled details about the positions of mirror
modules and their geometry.
The data in the position file is assumed to be columnar with the following format:
#index   row    X/m    Y/m    R/m    alpha/deg.
...

index: a running number identifying the mirror module within a petal.
row: the a running number for which row (ring) this mirror module belongs to (n.b. this is used as an index inot the geomfile).
X: The x-coordinate of the mirror module (ignored because redundant with R,alpha)
Y: The y-coordinate of the mirror module (ignored because redundant with R,alpha)
R: The radial position of the mirror module
alpha: The angular position of the mirror module

Similarly the format of the geomfile is:
#row  L/m  rad_h/m  rad_m/m  rad_p/m  width/m
...
row: running index for the rows/rings
L: The length of the plates for this ring
rad_h: The radius at the "hyperbolic" end of the optic. At the detector end.
rad_m: The radius at the midpoint of the optic. This is the reference.
rad_p: The radius at the "parabolic" and of the optic. At the source end.
width: pore width ?

Example: ATHENA_1pore.instr porenumber=3
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| FL | m | The focal length of the optical system | 12 |
| optics_dist | m | The distance between souce and optic. In space this would be quite large :-). | 10 |
| SRC_POS_X | m | Displacement of source along X | 0 |
| SRC_POS_Y | m | Displacement of source along Y | 0 |
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
| E0 | keV | Central energy of X-rays | 5 |
| dE | keV | Half spread of energy spectrum to be emitted from source | 0.001 |
| porenumber |  | Actually the mirror module number. | 1 |
| positionfile |  | File containing the pore/mirror module positions. | "ATHENA_mm_1petal.dat" |
| geomfile |  | File which contains the geometry of the pores (i.e. radii,lengths) | "ATHENA_rings_1_20.dat" |
| Hyper |  | If non-zero the secondary mirror (hyperbolic) is active. Useful for debugging. | 1 |
| Para |  | If non-zero the primary mirror is acive (parabolic) is active. Useful for debugging. | 1 |

## Links

- [Source code](ATHENA_1pore.instr) for `ATHENA_1pore.instr`.
- <a href="http://www.cosmos.esa.int/web/athena">The ATHENA web pages @ ESA</a>

---
