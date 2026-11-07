# The `Test_mcplot_loader` Instrument

*McStas: Regression test of the mcplot data loader (mccodelib) for ASCII and NeXus output.*

## Identification

- **Site:** Tests_other
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** October 2026

## Description

```text
A 0D, a 1D and a non-square 2D monitor for test_mcplot_loader.py, which
scans 'w' with and without --format=NeXus and checks that mcplot's data
loader reads the same scan curves, per-step monitors and single-run
monitors from both formats.
```

## Examples



## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| w | m | Width of the source focusing window | 0.05 |

## Links

- [Source code](Test_mcplot_loader.instr) for `Test_mcplot_loader.instr`.
- test_mcplot_loader.py next to this instrument.

---
