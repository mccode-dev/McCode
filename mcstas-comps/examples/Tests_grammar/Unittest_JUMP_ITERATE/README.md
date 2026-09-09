# The `Unittest_JUMP_ITERATE` Instrument

*McStas: JUMP ITERATE unittest*

## Identification

- **Site:** Tests_grammar
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 28th, 2021

## Description

```text
JUMP ITERATE unittest

The instrument uses JUMP ITERATE to let each particle make n='jumps' returns
to the monitor. The particles have unit weight and hence the intensity scales
with the simulated statistics. (Use input parameter Ncount for setting the stats).
```

## Examples

- **Test: Ncount=1000 jumps=10 Detector: PSD_I=10000**
- **Test: Ncount=1000 jumps=20 Detector: PSD_I=20000**
- **Test: Ncount=100000 jumps=10 Detector: PSD_I=1e6**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Ncount | 1 | Specify statistics | 1000 |
| jumps | 1 | Number of jumps | 10 |

## Links

- [Source code](Unittest_JUMP_ITERATE.instr) for `Unittest_JUMP_ITERATE.instr`.
- <reference/HTML link>

---
