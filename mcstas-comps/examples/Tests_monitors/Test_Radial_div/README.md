# The `Test_Radial_div` Instrument

*McStas: Test of the contributed Radial_div monitor*

## Identification

- **Site:** Tests_monitors
- **Author:** P. Willendrup
- **Origin:** DTU
- **Date:** 20. October 2026

## Description

```text
A small, weakly divergent beam is recorded by a PSD_monitor and by a Radial_div
monitor of the same size. With the beam divergence well within maxdiv_r and
all wavelengths within [Lmin, Lmax], both monitors record the same intensity.
Narrowing [Lmin, Lmax] makes Radial_div count only part of the beam.
```

## Examples

- **Test: Test_Radial_div.instr Lmin=0.5 Lmax=10 Detector: PSD_I=0.000100531**
- **Test: Test_Radial_div.instr Lmin=0.5 Lmax=10 Detector: RadDiv_I=0.000100531**
- **Test: Test_Radial_div.instr Lmin=4 Lmax=6 Detector: RadDiv_I=2.51059e-05**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Lmin | AA | Minimum wavelength counted by Radial_div | 0.5 |
| Lmax | AA | Maximum wavelength counted by Radial_div | 10 |
| maxdiv_r | deg | Maximal radial divergence counted by Radial_div | 2 |

## Links

- [Source code](Test_Radial_div.instr) for `Test_Radial_div.instr`.

---
