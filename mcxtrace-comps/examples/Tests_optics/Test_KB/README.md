# The `Test_KB` Instrument

*McXtrace: Test of Kirkpatrick Baez (KB) mirrors*

## Identification

- **Site:** Tests_optics
- **Author:** Antoine Padovani and Emmanuel Farhi
- **Origin:** SOLEIL
- **Date:** March 2022

## Description

```text
Two curved mirrors in a Kirkpatrick Baez geometry, i.e. two perpendicular
mirrors, one shared focus, and a compact X-ray beam for fine-scale experiments.
Default parameters are set for a focusing f_t=f_s= 10 m.
The KB should satisfy f_t = (R sin theta)/2 and f_s = R / (2 sin theta) where
theta is the tilt angle, f_t and f_s are tangential and sagittal focus
distances, and both mirrors are bent along z (tangential).
```

## Examples

- **Test: Test_KB L=12 Detector: psd_I=0.677509**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L | m | Distance to the PSD detector (focus) | 11.5 |
| theta | rad | Mirror glancing angle | 0.003 |
| R | m | Mirror curvature radius | 6.66e3 |
| mirror_length | m | Length of each KB mirror | 0.5 |
| source2kb | m | Distance from source to KB | 31.5 |
| E0 | keV | Nominal energy at the source | 23.32 |
| dE | keV | Energy half-bandwidth at the source | 1 |

## Links

- [Source code](Test_KB.instr) for `Test_KB.instr`.
- https://github.com/mccode-dev/Schools/tree/master/2023/SOLEIL_May_2023/Day2_Wednesday_May_10th/5_Optics/2_KB_mirrors

---
