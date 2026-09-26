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

## Examples

- **Test: lambda=1 Detector: Sph_sum_I=4.93378e+09**
- **Test: lambda=1 Detector: Sph_front_I=3.35878e+08**
- **Test: lambda=1 Detector: Sph_sample_I=7.28574e+08**
- **Test: lambda=1 Detector: Sph_back_I=2.73695e+08**

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
