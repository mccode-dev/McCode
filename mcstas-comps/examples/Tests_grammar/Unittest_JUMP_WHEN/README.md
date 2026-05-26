# The `Unittest_JUMP_WHEN` Instrument

*McStas: JUMP WHEN unittest*

## Identification

- **Site:** Tests_grammar
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Oct 28th, 2021

## Description

```text
JUMP WHEN unittest.

One unit of intensity is emitted from a 1x1 m. The particles generated in
one source quadrant is repteated 'jumps' times. Resulting intensity should be:
1 + 0.25*jumps;
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| jumps | 1 | Number of jumps to do | 10 |
| Pp0 | 1 | Dummy input parameter used internally | 0 |

## Links

- [Source code](Unittest_JUMP_WHEN.instr) for `Unittest_JUMP_WHEN.instr`.
- <reference/HTML link>

---
