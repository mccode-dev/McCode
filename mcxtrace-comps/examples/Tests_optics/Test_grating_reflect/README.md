# The `Test_grating_reflect` Instrument

*McXtrace: Unit-test instrument for the Grating reflect component.*

## Identification

- **Site:** Tests_optics
- **Author:** Stephane Bac, Antoine Padovani
- **Origin:** Synchrotron Soleil
- **Date:** Jul 21st 2022

## Description

```text
Simply a bending magnet illuminating a reflection grating.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Source's center of emitted energy spectrum | 1 |
| dE | keV | Source's half-width of emitted energy spectrum | 0.1 |
| angle_grating_norm | deg. | Angle between the norm of the grating and the incident ray | 88 |
| number_lines_per_mm |  | Number of lines pr mm of the grating | 100 |
| order |  | The target order of the grating | 0 |
| dphi | deg. | Range of diffraction angle that is to be simulated -d_phi/2 ; d_phi/2 | 1 |

## Links

- [Source code](Test_grating_reflect.instr) for `Test_grating_reflect.instr`.

---
