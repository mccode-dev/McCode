# The `Test_Sqw` Instrument

*McStas: Test output of Isotropic_Sqw on a spherical monitor.*

## Identification

- **Site:** Tests_samples
- **Author:** P. Willendrup
- **Origin:** DTU
- **Date:** October 1st 2020

## Description

```text
A test instrument for testing Isotropic_Sqw output on a spherical monitor.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength emitted from source, 1% wl-interval around. | 1 |
| L1 | m | Source-sample distance | 10 |
| directbeam | 1 | Suppress direct beam or not | 0 |
| sqw_coh | str | Sqw material definition | "Rb_liq_tot.sqw" |
| SPLITS | 1 | Number of SPLIT's before sample | 1 |

## Links

- [Source code](Test_Sqw.instr) for `Test_Sqw.instr`.

---
