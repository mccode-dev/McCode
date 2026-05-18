# The `Test_KB` Instrument

*McXtrace: Test of Kirkpatrick Baez (KB) mirrors*

## Identification

- **Site:** Tests_optics
- **Author:** Antoine Padovani
- **Origin:** SOLEIL
- **Date:** March 2022

## Description

```text
Two curved mirrors in a Kirkpatrick Baez geometry. Default parameters
are set for a focusing f_m=f_s= 10 m.
The KB should satisfy f_m=R.sin(theta/2) and f_s=R/2.sin(theta)

Example: Test_KB L=12
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L | m | Distance to the PSD detector, with a shift by 1.5 m | 12 |
| theta | rad | Mirror glancing angle | 0.003 |
| R | m | Mirror curvature radius | 6.66e3 |

## Links

- [Source code](Test_KB.instr) for `Test_KB.instr`.
- https://github.com/mccode-dev/Schools/tree/master/2023/SOLEIL_May_2023/Day2_Wednesday_May_10th/5_Optics/2_KB_mirrors

---
