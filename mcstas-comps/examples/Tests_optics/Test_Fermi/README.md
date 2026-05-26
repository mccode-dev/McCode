# The `Test_Fermi` Instrument

*McStas: Cross comparison of Fermi Chopper components*

## Identification

- **Site:** Tests_optics
- **Author:** E. Farhi [farhi@ill.fr]
- **Origin:** ILL
- **Date:** Sept 1st, 2008

## Description

```text
Cross comparison of Fermi Chopper components, using McStas and
contributed components, as well as rotating collimator approximations. It
shows that all implementations are equivalent. However, approximating rotating
guide are 30% faster than McStas Fermi chopper.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Fermi | 1 | Choice of Fermi chopper component to test, with 1=FermiChopper, 3=FermiChopper_ILL, 4=rotating Guide_gravity, 5=rotating Guide_channeled | 1 |
| lambda | AA | Mean wavelength from source | 3.39 |
| width_FC | m | total slit pack width | 0.044 |
| height_FC | m | height of FC slit pack | 0.064 |
| length_FC | m | length of FC slit pack | 0.012 |
| FC_Hz | Hz | rotation speed. Omega=2*PI*nu in rad/s, nu*60 in rpm | 100 |
| Nslit_FC |  | Number of slits in FC slit package | 120 |
| d_SF | m | distance from Source to FC center | 3 |
| d_FD | m | distance from FC center to Detector | 3 |
| phase | deg | FC phase. Use -0 for automatic | 271.92 |
| time_to_arrival |  |  | 0 |
| time_window_width |  |  | 0 |

## Links

- [Source code](Test_Fermi.instr) for `Test_Fermi.instr`.

---
