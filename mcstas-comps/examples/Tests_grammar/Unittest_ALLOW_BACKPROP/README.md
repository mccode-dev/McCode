# The `Unittest_ALLOW_BACKPROP` Instrument

*McStas: Unittest for ALLOW_BACKPROP macro*

## Identification

- **Site:** Tests_grammar
- **Author:** Peter Willendrup
- **Origin:** ESS
- **Date:** February 2026

## Description

```text
Unittest to ensure that the macro ALLOW_BACKPROP works correctly.

The test instrument includes two monitors, with a selectable ALLOW_BACKPROP in between.
The second monitor is placed physically "earlier" than the first, but logically after.

1) With Backprop=0 the second monitor (closer to the source) should yield a signal of 0
2) With Backprop=1 the second monitor should measure a signal since backpropagation is allowed
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Backprop | 1 | Flag to enable the ALLOW_BACKPROP macro | 0 |

## Links

- [Source code](Unittest_ALLOW_BACKPROP.instr) for `Unittest_ALLOW_BACKPROP.instr`.
- A reference/HTML link for more information

---
