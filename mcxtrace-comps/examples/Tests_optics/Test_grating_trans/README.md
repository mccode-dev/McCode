# The `Test_grating_trans` Instrument

*McXtrace: Template/test instrument for Grating_trans*

## Identification

- **Site:** Tests_optics
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Aug 21

## Description

```text
A simple test insrument for the transmission grating component. Two examples
of gratings are included matching those in the Chandra X-ray Observatory
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Central energy to emit from the source. | 1 |
| dE | kEV | Half energy spread of source. | 0.001 |
| GD | m | Distance from grating to detector. | 1 |
| Rx | deg | Rotation of grating around the X-axis. | 0 |
| Ry | deg | Rotation of grating around the Y-axis. | 0 |
| Rz | deg | Rotation of grating around the Z-axis. | 0 |
| MEG |  | Flag to chosse between the two included gratings. 1: medium energy grating (MEG), 0: high energy grating (HEG). | 0 |

## Links

- [Source code](Test_grating_trans.instr) for `Test_grating_trans.instr`.
- https://cxc.harvard.edu/proposer/POG/html/chap8.html#tb:hetg-params

---
