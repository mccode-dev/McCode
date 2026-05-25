# The `Test_single_magnetic_crystal` Instrument

*McStas: Unit test instrument for magnetic single crystal*

## Identification

- **Site:** Tests_samples
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Jan 2020

## Description

```text
A very simple unit test/example instrument for the experimental Single_magnetic_crystal
component. It is a very simple implementation of a Laue camera using 4PI spherical
monitors.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L0 | AA | Centre wavelength to simulate | 4 |
| dL | AA | Half wavelength spread to sample | 3.91 |
| OM | deg | Sample rotation | 0 |
| TT | deg | Rotation of detector arm | 0 |
| PX |  | x-component of initial polarization | 0 |
| PY |  | y-component of initial polarization | 1 |
| PZ |  | z-component of initial polarization | 0 |
| MOS | arcmin | Isotropic mosaic of the crystal. | 100 |
| QMIN | AA^-1 | lower boundary of momentum transfer range to generate hkls in | 0.1 |
| QMAX | AA^-1 | upper boundary of momentum transfer range to generate hkls in | 5 |

## Links

- [Source code](Test_single_magnetic_crystal.instr) for `Test_single_magnetic_crystal.instr`.
- A reference/HTML link for more information

---
