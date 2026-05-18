# The `Test_SX` Instrument

*McStas: Test output of Single Crystal on a spherical monitor.*

## Identification

- **Site:** Tests_samples
- **Author:** P. Willendrup
- **Origin:** DTU
- **Date:** October 1st 2020

## Description

```text
A test instrument to compare Monitor_nD output against basic 1D and 2D monitors.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Central wavelength emitted from source | 5 |
| dlambda | Angs | Witdth of wavelength spectrom emitted from source | 9.8 |
| L1 | m | Source-sample distance | 30 |
| directbeam | 1 | Suppress direct beam or not | 0 |
| beamstop |  | Toggle beamstop | 0 |
| reflections | str | File name for reflection list | "YBaCuO.lau" |
| SPLITS | 1 | Number of SPLIT's before sample | 1 |
| order |  | Maximum order of multiple scattering, Single_crystal | 0 |
| inc | barns | Incoherent cross-section | 0 |
| trans |  | Fraction of statistics assigned to transmitted beam | 0.001 |
| omega | deg | Rotation angle of sample | 0 |

## Links

- [Source code](Test_SX.instr) for `Test_SX.instr`.

---
