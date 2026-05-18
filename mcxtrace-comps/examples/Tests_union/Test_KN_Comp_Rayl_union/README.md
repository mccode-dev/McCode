# The `Test_KN_Comp_Rayl_union` Instrument

*McXtrace: An example using Union with Klein-Nishina, Rayleigh, or Compton scattering*

## Identification

- **Site:** Templates
- **Author:** E. B. Knudsen (erkn@united-neux.eu)
- **Origin:** United Neux
- **Date:** Aug 13th 2021

## Description

```text
An example instrument that uses either Klein-Nishina, Rayleigh, or Compton on a sphere
filled with whatever material is indicated by the atom number
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | incident energy | 12 |
| PROCESS |  | which process to run the test for, 0: Klein-Nishina, 1: Rayleigh, 2: Compton. | 0 |
| atomno |  | atom number of the material to scatter on. | 29 |

## Links

- [Source code](Test_KN_Comp_Rayl_union.instr) for `Test_KN_Comp_Rayl_union.instr`.

---
