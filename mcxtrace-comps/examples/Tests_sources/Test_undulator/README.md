# The `Test_undulator` Instrument

*McXtrace: Unit test instrument for the Undulator component*

## Identification

- **Site:** Tests_sources
- **Author:** Erik B Knudsen <erkn@fysik.dtu.dk>
- **Origin:** DTU Physics
- **Date:** Aug 2016

## Description

```text
A simple undulator with parameters as in Kim, 1989 (sec. 4).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| minh | 1 | lowest harmonic to include in the simulation | 1 |
| maxh | 1 | highest harmonic to include in the simulation | 1 |
| FAST | 0/1 | Use a faster integration scheme | 0 |

## Links

- [Source code](Test_undulator.instr) for `Test_undulator.instr`.

---
