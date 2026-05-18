# The `Test_Pol_Tabled` Instrument

*McStas: Test the tabled magnetic field option*

## Identification

- **Site:** Tests_polarization
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** 2024

## Description

```text
A test instrument to check that supplying a magnetic field as a vector field point cloud
is working. The field referred to by the default input parameter MF is 10 \mu T along the z-axis
at entry and flips to l0 \mu T along the negative z-axis halfway through
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| MF | string | Field definition file | "flipfield.dat" |
| zdepth | m | Z-dimension of field | 1 |
| interpol_method | string | Choice of interpolation method "kdtree" (default on CPU) / "regular" (default on GPU) | "default" |

## Links

- [Source code](Test_Pol_Tabled.instr) for `Test_Pol_Tabled.instr`.
- [Additional information](Test_Pol_Tabled.md)
- A reference/HTML link for more information

---
