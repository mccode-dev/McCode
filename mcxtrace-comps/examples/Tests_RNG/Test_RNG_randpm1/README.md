# The `Test_RNG_randpm1` Instrument

*McXtrace: Instrument to get basic test output from random number generator.*

## Identification

- **Site:** Tests_RNG
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 2nd, 2020

## Description

```text
Instrument to get basic test output from random number generator.

The instrument generates 2 randon numbers pr. particle using randpm1(),
encoded into the particle x and y coordinates.

For a low statistics, one may e.g. check that setting the seed gives
varied output or fixing the seed gives fixed output·
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Ncount | 1 | Specify statistics | 1000 |
| seed | 1 | Specify RNG seed | 0 |

## Links

- [Source code](Test_RNG_randpm1.instr) for `Test_RNG_randpm1.instr`.
- <reference/HTML link>

---
