# The `PSI_ICON_porous` Instrument

*McStas: PSI ICON imaging beamline with a porous quartz sample, water in the pores below fill_y*

## Identification

- **Site:** PSI
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** Nov 2026

## Description

```text
PSI_ICON_beamline and PSI_ICON_camera (see %include below) with a porous quartz
cylinder (PowderN_porous), pore channels from pores.off (in the data folder, made with Tests_samples/Test_PowderN_porous/make_pores.py),
water in the pores below fill_y.
All PSI_ICON_beamline parameters are available as well.
```

## Examples

- **Test: PSI_ICON_porous.instr -y Detector: VarDetectorZoom_I=759312**
- **Test: PSI_ICON_porous.instr -y matrix=stdlib::SiO2-alpha_sg154_AlphaQuartz.ncmat Detector: VarDetectorZoom_I=758469**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| matrix | str | Reflection file or NCrystal cfg-string of the porous matrix | "SiO2_quartza.laz" |
| pores | str | Pore mesh file (Tests_samples/Test_PowderN_porous/make_pores.py), "NULL" for none | "pores.off" |
| fill_y | m | Water level in the pores, local y (sample spans +-0.005) | 0 |
| water | 1 | 1: water below fill_y, 0: void pores | 1 |

## Links

- [Source code](PSI_ICON_porous.instr) for `PSI_ICON_porous.instr`.

---
