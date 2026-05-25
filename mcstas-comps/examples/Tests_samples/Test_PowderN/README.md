# The `Test_PowderN` Instrument

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
| directbeam | 1 | Suppress direct beam or not | 0 |
| reflections | str | List of powder reflections | "Fe.laz" |
| SPLITS | 1 | Number of SPLIT's before sample | 1 |
| frac_c | 1 | Fraction of stats assigned to coherent scattering | 0.8 |
| frac_i | 1 | Fraction of stats assigned to incoherent scattering | 0.1 |
| frac_t | 1 | Fraction of stats assigned to unscattered, "direct beam" | 0.1 |
| d_phi | deg | d_phi focusing setting in PowderN | 0 |

## Links

- [Source code](Test_PowderN.instr) for `Test_PowderN.instr`.

---
