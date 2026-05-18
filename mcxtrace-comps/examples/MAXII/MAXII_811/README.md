# The `MAXII_811` Instrument

*McXtrace: XAFS and surface diffraction, Materials Science Beamline 811 at MAX-lab.*

## Identification

- **Site:** MAXII
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Jan 23, 2011

## Description

```text
Fed by a wiggler, this beamline operates in monochromatic mode with an
operational energy tunable between 2.3 – 20 keV (0.6 - 5.3 AA).
The wavelength is chosen by a double monochromator crystal setup where either
Si 111 or Si 311 may be chosen. Furthermore a double mirror
setup may be used to to focus the beam onto a sample. The rotation of the
first mirror defines the position of the second which is computed automatically.
If mirrors are in the monochromators are automatically reposistioned to
accomadate the deflected beam.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Emin | keV | Minmium energy of radiation emitted from wiggler. | 1e-3 |
| Emax | keV | Maxmium energy of radiation emitted from wiggler. | 12 |
| prim_h | m | Horizontal opening of primary slits. | .40e-3 |
| prim_v | m | Vertical opening of primary slits. | 0.6e-3 |
| xafs_h | m | XAFS station horizontal slit opening | 3e-3 |
| xafs_v | m | XAFS station vertical   slit opening | 3e-3 |
| M1 |  | Is mirror 1 in the beam? | 0 |
| M1_R | m | Radius of curvature of mirror 1. | 2000 |
| M1_pitch | deg | Central glancing angle of mirror 1. | 0 |
| M2 |  | Is mirror 2 in the beam? | 0 |
| M2_R | m | Radius of curvature of mirror 2. | 800 |
| M2_pitch | deg | Central glancing angle of mirror 2. | 0 |
| theta | deg | Scattering angle of monochromators | 14.226 |
| TTH | deg | Angle at which to put the surface diffraction detector | 0 |
| ATT1 |  | Should attenuation filter 1 be in the beam? | 0 |
| ATT2 |  | Should attenuation filter 2 be in the beam? | 0 |
| ATT3 |  | Should attenuation filter 3 be in the beam? | 0 |
| XAFS |  | Flag to activate/inactivate the XAFS station | 0 |

## Links

- [Source code](MAXII_811.instr) for `MAXII_811.instr`.

---
