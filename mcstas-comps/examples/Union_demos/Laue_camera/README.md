# The `Laue_camera` Instrument

*McStas: Union example: Laue camera*

## Identification

- **Site:** Union_demos
- **Author:** Mads Bertelsen
- **Origin:** University of Copenhagen
- **Date:** September 2015

## Description

```text
Laue camera using Union components for the sample
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| delta_d_d |  | Lattice spacing variation in sample | 1e-4 |
| mosaic | arcmin | Sample mosaicity | 5 |
| lam0 | AA | Source mean wavelength | 7 |
| dlam | AA | Source wavelength spread | 5 |
| radius | m | Sample radius | 0.01 |
| height | m | Sample height | 0.01 |
| x_rotation_geometry | deg | Rotates union geometry component around x | 0 |
| y_rotation_geometry | deg | Rotates union geometry component around y | 0 |
| x_rotation_geometry_ref | deg | Rotates reference component around x | 0 |
| y_rotation_geometry_ref | deg | Rotates reference component around y | 0 |
| x_rotation_process | deg | Rotates crystal process (crystal orientation), x | 0 |
| y_rotation_process | deg | Rotates crystal process (crystal orientation), y | 0 |
| z_rotation_process | deg | Rotates crystal process (crystal orientation), z | 0 |
| geometry_interact |  | p_interact of Union sample | 0 |

## Links

- [Source code](Laue_camera.instr) for `Laue_camera.instr`.

---
