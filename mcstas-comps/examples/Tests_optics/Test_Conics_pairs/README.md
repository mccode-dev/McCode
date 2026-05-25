# The `Test_Conics_pairs` Instrument

*McStas: Unit test instrument for Conics Pairs components*

## Identification

- **Site:** Tests_optics
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** Dec '21

## Description

```text
Example instrument that shows some ways of using Conics_Ph and Conics_EH
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| OPTIC |  | Flag to choose between 0: no optic, 1: EH pair, 2: PH pair | 1 |
| ssize | m | Source radius | 1e-6 |
| fs | m | Distance betwee nsource and optic mid plane | 10 |
| fi | m | Distance between optics mid plane and focal point. | 10 |
| R0 | 1 | Mirror substrate reflectivity | 0.99 |
| m | 1 | m-value of supermirrors | 3 |
| W | AA-1 | Width of supermirror cut-off | 0.003 |
| alpha | AA | Slope of reflectivity for reflectivity curve approximation | 6.07 |
| nshells | 1 | Number of Wolter-optic shells | 4 |
| rmin |  |  | 0.0031416 |
| rmax |  |  | 0.05236 |

## Links

- [Source code](Test_Conics_pairs.instr) for `Test_Conics_pairs.instr`.
- <reference/HTML link>

---
