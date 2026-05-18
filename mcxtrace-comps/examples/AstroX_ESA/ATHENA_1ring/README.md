# The `ATHENA_1ring` Instrument

*McXtrace: Single pore version of the ATHENA SPO-optic in use as telescope.*

## Identification

- **Site:** AstroX_ESA
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk> & Desiree D. M. Ferreira <desiree@space.dtu.dk> (email)
- **Origin:** DTU Physics/DTU Space
- **Date:** 12/12/2016

## Description

```text
A model of the ATHENA-telescope using a single ring as optical element. That means to make use of this instrument
it is necessary to run a series of simulation while varying the input parameter porenumber.
At present, the porenumber really means ring number.

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

Example: ATHENA_1ring.instr shellnumber=3
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
| reflectivity |  | Data file containing reflectivities (such as from IMD) | "mirror_coating_unity.txt" |
| E0 | keV | Central energy of X-rays | 5 |
| dE | keV | Half spread of energy spectrum to be emitted from source | 0.001 |
| shellnumber |  | Actually the ring number. | 1 |
| ringfile |  | File which contains details for the meta shells. | "ref_design_breaks.txt" |
| geomfile |  |  | "ATHENA_rings_1_20.dat" |

## Links

- [Source code](ATHENA_1ring.instr) for `ATHENA_1ring.instr`.
- <a href="http://www.cosmos.esa.int/web/athena">The ATHENA web pages @ ESA</a>

---
