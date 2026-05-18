# The `Test_PowderN_concentric` Instrument

*McStas: Test output of PowderN on a spherical monitor.*

## Identification

- **Site:** Tests_samples
- **Author:** P. Willendrup
- **Origin:** DTU
- **Date:** October 1st 2020

## Description

```text
A test instrument for PowderN output on a spherical monitor.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength emitted from source, 1% wl-interval around. | 1 |
| L1 | m | Source-sample distance | 10 |
| directbeam | 1 | Suppress direct beam or not | 1 |
| reflections | str | List of powder reflections | "Fe.laz" |
| SPLITS | 1 | Number of SPLIT's before sample | 1 |

## Links

- [Source code](Test_PowderN_concentric.instr) for `Test_PowderN_concentric.instr`.

---
