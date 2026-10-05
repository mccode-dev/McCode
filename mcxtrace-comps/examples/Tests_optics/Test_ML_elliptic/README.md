# The `Test_ML_elliptic` Instrument

*McXtrace: Unit test instrument for Multilayer_elliptic*

## Identification

- **Site:** Tests_optics
- **Author:** Your name (email)
- **Origin:** Your institution
- **Date:** Current Date

## Description

```text
Tests the correct working of the mulitlyer_elliptic component both using a
reflectivity file and the kinematical approximation.
```

## Examples

- **Test: Test_ML_elliptic.instr S1=1 S2=2 gamma=1.2 fromfile=0 Detector: emon1_I=6.91956e-21**
- **Test: Test_ML_elliptic.instr S1=1 S2=2 gamma=1.2 fromfile=1 Detector: emon1_I=4.61321e-21**
- **Test: Test_ML_elliptic.instr S1=1 S2=2 gamma=1.2 fromfile=1 fxw=2e-2 fyh=1e-3 Detector: emon1_I=9.22372e-08**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| S1 | m | Distance from source focus to multilayer centre. | 1 |
| S2 | m | Distance from multilayer centre to focal point | 2 |
| gamma | deg | Design glancing angle at centre. | 1.2 |
| fromfile |  | If nonzero read reflectivity number from a datafile ("reflectivity.txt") | 1 |
| fxw | m | width of the beam | 1e-9 |
| fyh | m | height of the beam | 1e-9 |

## Links

- [Source code](Test_ML_elliptic.instr) for `Test_ML_elliptic.instr`.

---
