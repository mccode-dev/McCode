# The `Test_Air` Instrument

*McXtrace: Unit test for the Air component*

## Identification

- **Site:** Tests_samples
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** 

## Description

```text
This instrument checks whether the Air component works as intended.
A point source illuminates  the central part of a rectangular volume of air ( 0.02 x 0.02 x Lair  m^3 ).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| FRAC |  | Fraction of statistics to use for Air-scatter. | 0.5 |
| E0 | keV | Central energy to be emitted from the source. | 12.3984 |
| dE_E |  | Relative full bandwidth of the source. | 0.1 |
| Lair | m | The length of air to be traversed by the beam. | 1 |
| MXDIV |  | Maximal divergence to allow in divergence monitors. | 1 |
| AIR |  | Flag to enable (nonzero) / disable (0) the block of Air. | 0 |

## Links

- [Source code](Test_Air.instr) for `Test_Air.instr`.

---
