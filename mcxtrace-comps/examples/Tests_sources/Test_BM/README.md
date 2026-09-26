# The `Test_BM` Instrument

*McXtrace: Test instrument for the Bending_magnet component*

## Identification

- **Site:** Tests_sources
- **Author:** Erik B Knudsen (erkn@fysik.dtu.dk)
- **Origin:** DTU Physics
- **Date:** 07/05/2013

## Description

```text
This is a simple test-instrument for the bending magnet component.
```

## Examples

- **Test: Test_BM.instr SOURCE=0 e0=12.5 de=0.5 Detector: emon_I=2.93138e+13**
- **Test: Test_BM.instr SOURCE=1 e0=12.5 de=0.5 Detector: emon_I=1.28549e+15**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| SOURCE | 0/1 | select a BM (0) or a Wiggler (1) source. | 0 |
| e0 | keV | Central energy of the interval to be looked at | 25 |
| de | keV | Half-width of energy interval | 24.9 |

## Links

- [Source code](Test_BM.instr) for `Test_BM.instr`.

---
