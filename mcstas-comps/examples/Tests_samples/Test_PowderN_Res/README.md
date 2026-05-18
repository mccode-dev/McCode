# The `Test_PowderN_Res` Instrument

*McStas: Idealized powder diffractometer, to illustrate the difference between 'banana,theta' and 'banana,divergence' in Monitor_nD.*

## Identification

- **Site:** Tests_samples
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** 20160309

## Description

```text
Idealized powder diffractometer, to illustrate the difference between 'banana,theta' and 'banana,divergence' in Monitor_nD. Includes a Na2Ca3Al2F14 powder sample.

The beam from the source is very low-divergent, to allow studying effects at the sample "only".

By default, absorption in the sample is effectively suppressed.

Example: Lambda0=2.5667 dLambda=0.01 radius=0.001,0.021 TT=71.8 D_PHI=6 SPLITS=117 Distance=30 sig_abs=1e-09 -N21
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lambda0 | AA | Central wavelength of the source | 2.5667 |
| dLambda | AA | Width of wavelength band from the source | 0.01 |
| radius | m | Radius of cylindrical powder sample | 0.01 |
| TT | deg | Two-theta (scattering angle) of small banana detectors | 72 |
| D_PHI | deg | d_phi focusing around detector "plane" in degrees | 6 |
| SPLITS | 1 | SPLIT statistics booster at sample position | 117 |
| Distance | m | Distance between source and sample | 30 |
| sig_abs | barns | Absorption cross-section in the sample | 1e-9 |

## Links

- [Source code](Test_PowderN_Res.instr) for `Test_PowderN_Res.instr`.
- A reference/HTML link for more information

---
