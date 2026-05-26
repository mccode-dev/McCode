# The `Test_Monitor_nD` Instrument

*McStas: Test Monitor_nD against basic monitor*

## Identification

- **Site:** Tests_monitors
- **Author:** P. Willendrup
- **Origin:** DTU
- **Date:** 30. September 2020

## Description

```text
A test instrument to compare Monitor_nD output against basic 1D and 2D monitors.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength at monochromator, computed from DM and THETA_M if left as 0. | 1 |
| L1 | m | Source-sample distance | 10 |
| bins |  | Number of bins on monitors | 100 |
| xw | m | Width of detector | 0.11 |
| yh | m | Height of detector | 0.16 |
| hdiv | deg | Horizontal divergence measured by detector | 2 |
| vdiv | deg | Vertical divergence measured by detector | 2 |

## Links

- [Source code](Test_Monitor_nD.instr) for `Test_Monitor_nD.instr`.

---
